/*
 * MinisatSolver.h
 *
 *  Created on: 23-Nov-2020
 *      Author: sjash
 */

#ifndef DIMACSPARSER_H_
#define DIMACSPARSER_H_

#include <bits/stdc++.h>
#include <cassert>
#include <set>
#include <map>
#include <vector>
#include <stack>
#include <algorithm>
#include <functional>
#include <random>
using namespace std;

// ========================= Clause ==================
//literals and clause-id are kept as int driven by int datatype of the corresponding entries in minisat
class CNFClause {
private:
    set<int> literals;
    unsigned long long weight;
    bool hardClause;
    int uniqueId = -1; //uninitialized
    static int monotonicSeq;

    static int generateSeq(){
        int res = monotonicSeq;
        monotonicSeq ++;
        return (res);
    }
public:

    CNFClause(){//default blank constructor is needed if it is used in map
        //assert(false);
    }

    CNFClause(unsigned long long wt, bool hard, const set<int>& lit, int uID) {
        weight = wt;
        hardClause = hard;
        uniqueId = uID;
        literals.insert(lit.cbegin(), lit.cend());
    }

    CNFClause(unsigned long long wt, bool hard, const set<int>& lit) {
        weight = wt;
        hardClause = hard;
        uniqueId = generateSeq();
        literals.insert(lit.cbegin(), lit.cend());
    }

    CNFClause(const CNFClause& c) {
        literals = c.literals;
        weight = c.weight;
        hardClause = c.hardClause;
        uniqueId = c.uniqueId;
    }

    CNFClause & operator = (const CNFClause &c){

        if(this == &c){
            return (*this); //self assignment check
        }

        literals = c.literals;
        weight = c.weight;
        hardClause = c.hardClause;
        uniqueId = c.uniqueId;

        return (*this);
    }

    virtual ~CNFClause(){
        literals.clear();
    }

    void insertVar(int var) {
        literals.insert(var);
    }

    void insertVars(const CNFClause& c) {
        const set<int>& vars = c.literals;
        literals.insert(vars.cbegin(), vars.cend());
    }

    unsigned long long getWeight() const{
        return (weight);
    }

    bool isHard() const {
        return (hardClause ? true : false);
    }

    bool isPresent(int var) const {
        return ((literals.find(var) == literals.end()) ? false : true);
    }

    bool isSingleVariableClause() const {
        return ((1 == literals.size()) ? true : false);
    }

    bool isEmptyClause() const {
        return ((0 == literals.size()) ? true : false);
    }

    int getVariablesListSize() const {
        return (literals.size());
    }

    void deleteVar(int var) {
        literals.erase(var);
    }

    int getUniqueID() const{
        return (uniqueId);
    }

    int getUnitVar() const {
        if (!isSingleVariableClause()) {
            throw std::logic_error("");
        }
        return (*(literals.cbegin()));
    }

    const set<int>& getVars() const {
        return (literals);
    }

    bool isTautology() const {
        set<int>::const_iterator it;
        for(it = literals.cbegin(); it != literals.cend(); it++){
            if(literals.find(-(*it)) != literals.end()){
                return (true);
            }
        }
        return (false);
    }

    friend ostream &operator<<( ostream &output, const CNFClause &C ) {
        output << "c [" << C.uniqueId << " wt=" << C.weight << " " << (C.hardClause ? "H":"S") << "]:";
        set<int>::const_iterator it;
        for(it = C.literals.cbegin(); it != C.literals.cend(); it++){
            output << *it << " " ;
        }
        output << endl;
        return (output);
    }

    static void resetUniqueIdGenerateSeq(){
        monotonicSeq = 0;
    }
};

int CNFClause::monotonicSeq = 0;

// ========================= Weighted DIMACS parser ==================

class WeightedDimacsParser {
private:
    bool withWeight;
    int maxVarCount;
    int maxClauseCount;
    unsigned long long hardClauseIndicatorWeight;
    int currentParsedClauseIndex;
    int hardClauseCount;
    int softClauseCount;
    vector<CNFClause> originalClauses;
    void parseMetaHeaderLine(const char* line);
    void parseClauseLine(const char* line);
    map<int, CNFClause> clauseIdToClauseMap;

public:
    WeightedDimacsParser(bool iw=true);
    virtual ~WeightedDimacsParser();
    void parse(FILE* in, bool isDebug);
    int getOriginalVarCount() const {
        return (maxVarCount);
    }
    int getOriginalClauseCount() const {
        return (maxClauseCount);
    }
    int getHardClauseCount() const {
        return (hardClauseCount);
    }
    int getSoftClauseCount() const {
        return (softClauseCount);
    }
    const vector<CNFClause>& getOriginalClauses() const {
        return (originalClauses);
    }
    const CNFClause& getClause(int id) {
        if (clauseIdToClauseMap.find(id) != clauseIdToClauseMap.end()){
            //without above check, an object will be automatically created
            return (clauseIdToClauseMap[id]);
        }
        assert(false);
    }

    void randomShuffleClauses(){
        std::random_device r;
        std::mt19937_64 eng(r());
        shuffle(originalClauses.begin(), originalClauses.end(), eng);
    }

    friend ostream &operator<<( ostream &output, const WeightedDimacsParser &D ) {
        vector<CNFClause>::const_iterator it;
        for(it = D.originalClauses.cbegin(); it != D.originalClauses.cend(); it++){
            output << *it << std::endl;
        }
        return (output);
    }
};

WeightedDimacsParser::WeightedDimacsParser(bool iw) : withWeight(iw),
        maxVarCount(0), maxClauseCount(0),
        hardClauseIndicatorWeight(-1), currentParsedClauseIndex(0),
        hardClauseCount(0), softClauseCount(0){
}

WeightedDimacsParser::~WeightedDimacsParser() {
    originalClauses.clear();
}

void WeightedDimacsParser::parseMetaHeaderLine(const char* line) {
    int lineLen = strlen(line);
    char bufPerToken[256];
    int posInTokenBuf = 0;
    bool tokenParsingState = false;
    unsigned long long values[3];
    int currentParsedToken = 0;
    for (int i = 1; i < lineLen; i++) { //first char is known to be 'p' - thus starting with index 1
        if (('0' <= line[i]) && (line[i] <= '9')) {
            bufPerToken[posInTokenBuf] = line[i];
            posInTokenBuf++;
            tokenParsingState = true;
        } else {
            if (tokenParsingState) {
                assert(currentParsedToken <= 3);
                bufPerToken[posInTokenBuf] = '\0';
                values[currentParsedToken] = atoll(bufPerToken);
                currentParsedToken++;
            }
            tokenParsingState = false;
            posInTokenBuf = 0;
        }
    }
    if(withWeight){
        assert(3 == currentParsedToken);
        maxVarCount = values[0];
        maxClauseCount = values[1];
        hardClauseIndicatorWeight = values[2];
    }else{
        assert(2 == currentParsedToken);
        maxVarCount = values[0];
        maxClauseCount = values[1];
    }

}

void WeightedDimacsParser::parseClauseLine(const char* line) {

    if (currentParsedClauseIndex >= maxClauseCount) {
        throw std::logic_error("");
    }

    int lineLen = strlen(line);
    char bufPerToken[256];
    int posInTokenBuf = 0;
    bool tokenParsingState = false;
    bool firstTokenAsWeight = withWeight ? true : false;
    unsigned long long weight = 0;

    set<int> orgClause;
    for (int i = 0; i < lineLen; i++) {
        if ((line[i] != '+') && (line[i] != '-') && ((line[i] < '0') || ('9' < line[i]))) {
            if (tokenParsingState) {
                bufPerToken[posInTokenBuf] = '\0';
                long long token = atoll(bufPerToken);
                if (0 == token) { //terminating zero found
                    break;
                }
                if(firstTokenAsWeight){
                    assert(((unsigned long long)token) <= hardClauseIndicatorWeight );
                    weight = token;
                    firstTokenAsWeight = false;
                }else {
                    orgClause.insert(token);
                    /*if((token < 1) || (token > maxVarCount)){
                        cerr << token << " " << maxVarCount << " " << weight << " " << currentParsedClauseIndex << std::endl << std::flush;
                    }*/
                    assert((1 <= abs(token)) && (abs(token) <= maxVarCount));
                }
            }
            tokenParsingState = false;
            posInTokenBuf = 0;
        } else {
            bufPerToken[posInTokenBuf] = line[i];
            posInTokenBuf++;
            tokenParsingState = true;
        }
    }
    CNFClause c(weight, (weight == hardClauseIndicatorWeight) ? true : false , orgClause);
    originalClauses.push_back(c);
    clauseIdToClauseMap[c.getUniqueID()] = c;
    currentParsedClauseIndex++;
    if(weight == hardClauseIndicatorWeight){
        hardClauseCount ++;
    } else {
        softClauseCount ++;
    }
}

//FILE* is used for performance purpose - although it is c-styled
void WeightedDimacsParser::parse(FILE* in, bool isDebug) {
    char* line = NULL;
    size_t len = 0;
    int charRead = 0;

    assert(in != NULL);
    rewind(in);

    /*
     * getline() reads an entire line from stream, storing the address of
     the buffer containing the text into *lineptr.  The buffer is null-
     terminated and includes the newline character, if one was found.

     If *lineptr is set to NULL and *n is set 0 before the call, then
     getline() will allocate a buffer for storing the line.  This buffer
     should be freed by the user program even if getline() failed.

     Alternatively, before calling getline(), *lineptr can contain a
     pointer to a malloc(3)-allocated buffer *n bytes in size.  If the
     buffer is not large enough to hold the line, getline() resizes it
     with realloc(3), updating *lineptr and *n as necessary.
     */
    while ((charRead = getline(&line, &len, in)) != -1) {
        if (isDebug) {
            cout << "c [ClauseID:" << currentParsedClauseIndex << "]" << line << std::flush;
        }

        //trimming from beginning blank spaces
        int firstCharEffectiveStartIndex = 0;
        while ((' ' == line[firstCharEffectiveStartIndex]) || ('\t' == line[firstCharEffectiveStartIndex])) {
            firstCharEffectiveStartIndex++;
        }

        char firstChar = line[firstCharEffectiveStartIndex];
        if ((firstChar != '+') && (firstChar != '-') && (firstChar != 'p')
                && ((firstChar < '1') || ('9' < firstChar))) {
            continue; //ignorable line encountered e.g. comment line or line starting with % or line with single zero
        }

        if ('p' == firstChar) {
            if ((maxVarCount > 0) || (maxClauseCount > 0)) {
                throw std::logic_error(""); //only single line with 'p' allowed
            }
            parseMetaHeaderLine(line);
            continue;
        }

        parseClauseLine(&line[firstCharEffectiveStartIndex]);

        if (currentParsedClauseIndex >= maxClauseCount) {
            if (isDebug) {
                cout << "c Breaking from parsing" << endl << std::flush;
            }
            break;
        }
    }

    free(line);
}


#endif /* DIMACSPARSER_H_ */
