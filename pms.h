#ifndef _PMS_H_
#define _PMS_H_

#include "basis_pms.h"
#include "deci.h"
#include <sstream>
#include <array>

Satlike::Satlike(): problem_weighted(0),partial(0),
    max_clause_length(0), min_clause_length(0),
    num_vars(0), num_clauses(0), num_hclauses(0), num_sclauses(0)
{
    times(&start_time);//Shaswata

    unit_clause = NULL;
    var_lit = NULL;
    var_lit_count = NULL;
    clause_lit = NULL;
    clause_lit_count = NULL;
    score = NULL;
    var_neighbor = NULL;
    var_neighbor_count = NULL;
    time_stamp = NULL;
    neighbor_flag = NULL;
    temp_neighbor = NULL;

    org_clause_weight = NULL;
    clause_weight = NULL;
    sat_count = NULL;
    sat_var = NULL;
    clause_selected_count = NULL;
    best_soft_clause = NULL;

    hardunsat_stack = NULL;
    index_in_hardunsat_stack = NULL;
    softunsat_stack = NULL;
    index_in_softunsat_stack = NULL;

    unsatvar_stack = NULL;
    index_in_unsatvar_stack = NULL;
    unsat_app_count = NULL;

    goodvar_stack = NULL;
    already_in_goodvar_stack = NULL;

    cur_soln = NULL;
    best_soln = NULL;
    local_opt_soln = NULL;

    large_weight_clauses = NULL;
    soft_large_weight_clauses = NULL;
    already_in_soft_large_weight_stack = NULL;

    best_array = NULL;
    temp_lit = NULL;

    deci = NULL;
    //dimParser = NULL;
    //finalFormattedResult = NULL;
    generator = NULL;
}

void Satlike::settings(bool debug)
{
    //steps
    max_tries = 100000000;
    max_flips = INITIAL_MAX_FLIP;
    max_non_improve_flip = 10000000;

    large_clause_count_threshold=0;
    soft_large_clause_count_threshold=0;

    rdprob=0.01;
    hd_count_threshold=15;
    rwprob=0.1;
    smooth_probability=0.01;

    if((top_clause_weight/num_sclauses)>10000)
    {
        h_inc=300;
        softclause_weight_threshold=500;
    }
    else
    {
        h_inc=3;
        softclause_weight_threshold=0;
    }

    if (num_vars>2000)
    {
        rdprob=0.01;
        hd_count_threshold=15;
        rwprob=0.1;
        smooth_probability=0.0000001;
    }

    if(debug){
        cout << "h_inc=" << h_inc << " softclause_weight_threshold=" << softclause_weight_threshold <<endl;
        cout << "hd_count_threshold=" << hd_count_threshold << " smooth_probability=" << smooth_probability << endl;
        cout << "var_count=" << num_vars << " hclause_count=" << num_hclauses << " sclause_count=" << num_sclauses << endl;
        cout << "max_soft_wt=" << top_clause_weight   << " total_soft_wt=" << total_soft_weight << endl;
    }
}

void Satlike::update_hyper_param(int t, float sp,  int hinc, int eta, int max_search)
{
    hd_count_threshold=t;
    smooth_probability=sp;
    h_inc=hinc;
    softclause_weight_threshold=eta;
    max_non_improve_flip = max_search;
}

void Satlike::allocate_memory()
{
    int malloc_var_length = num_vars+10;
    int malloc_clause_length = num_clauses+10;

    unit_clause = new lit[malloc_clause_length];
    memset(unit_clause, 0, sizeof(lit) * malloc_clause_length);

    var_lit = new lit* [malloc_var_length];
    for(int i=0; i< malloc_var_length; ++i){
        var_lit[i] = NULL;
    }

    var_lit_count = new int [malloc_var_length];
    memset(var_lit_count, 0, sizeof(int) * malloc_var_length);

    clause_lit = new lit* [malloc_clause_length];
    for(int i=0; i< malloc_clause_length; ++i){
        clause_lit[i] = NULL;
    }

    clause_lit_count = new int [malloc_clause_length];
    memset(clause_lit_count, 0, sizeof(int) * malloc_clause_length);

    score = new long long [malloc_var_length];
    memset(score, 0, sizeof(long long) * malloc_var_length);

    var_neighbor = new int* [malloc_var_length];
    for(int i=0; i< malloc_var_length; ++i){
        var_neighbor[i] = NULL;
    }

    var_neighbor_count = new int [malloc_var_length];memset(var_neighbor_count, 0, sizeof(int) * malloc_var_length);
    time_stamp = new long long [malloc_var_length];memset(time_stamp, 0, sizeof(long long) * malloc_var_length);
    neighbor_flag = new int [malloc_var_length];memset(neighbor_flag, 0, sizeof(int) * malloc_var_length);
    temp_neighbor = new int [malloc_var_length];memset(temp_neighbor, 0, sizeof(int) * malloc_var_length);

    org_clause_weight = new long long [malloc_clause_length];memset(org_clause_weight, 0, sizeof(long long) * malloc_clause_length);
    clause_weight = new long long [malloc_clause_length];memset(clause_weight, 0, sizeof(long long) * malloc_clause_length);
    sat_count = new int [malloc_clause_length];memset(sat_count, 0, sizeof(int) * malloc_clause_length);
    sat_var = new int [malloc_clause_length];memset(sat_var, 0, sizeof(int) * malloc_clause_length);
    clause_selected_count = new long long [malloc_clause_length];memset(clause_selected_count, 0, sizeof(long long) * malloc_clause_length);
    best_soft_clause = new int [malloc_clause_length];memset(best_soft_clause, 0, sizeof(int) * malloc_clause_length);

    hardunsat_stack = new int [malloc_clause_length];memset(hardunsat_stack, 0, sizeof(int) * malloc_clause_length);
    index_in_hardunsat_stack = new int [malloc_clause_length];memset(index_in_hardunsat_stack, 0, sizeof(int) * malloc_clause_length);
    softunsat_stack = new int [malloc_clause_length];memset(softunsat_stack, 0, sizeof(int) * malloc_clause_length);
    index_in_softunsat_stack = new int [malloc_clause_length];memset(index_in_softunsat_stack, 0, sizeof(int) * malloc_clause_length);

    unsatvar_stack = new int [malloc_var_length];memset(unsatvar_stack, 0, sizeof(int) * malloc_var_length);
    index_in_unsatvar_stack = new int [malloc_var_length];memset(index_in_unsatvar_stack, 0, sizeof(int) * malloc_var_length);
    unsat_app_count = new int [malloc_var_length];memset(unsat_app_count, 0, sizeof(int) * malloc_var_length);

    goodvar_stack = new int [malloc_var_length];memset(goodvar_stack, 0, sizeof(int) * malloc_var_length);
    already_in_goodvar_stack = new int[malloc_var_length];memset(already_in_goodvar_stack, 0, sizeof(int) * malloc_var_length);

    cur_soln = new int [malloc_var_length];memset(cur_soln, 0, sizeof(int) * malloc_var_length);
    best_soln = new int [malloc_var_length];memset(best_soln, 0, sizeof(int) * malloc_var_length);
    local_opt_soln = new int[malloc_var_length];memset(local_opt_soln, 0, sizeof(int) * malloc_var_length);

    large_weight_clauses = new int [malloc_clause_length];memset(large_weight_clauses, 0, sizeof(int) * malloc_clause_length);
    soft_large_weight_clauses = new int [malloc_clause_length];memset(soft_large_weight_clauses, 0, sizeof(int) * malloc_clause_length);
    already_in_soft_large_weight_stack = new int [malloc_clause_length];memset(already_in_soft_large_weight_stack, 0, sizeof(int) * malloc_clause_length);

    best_array = new int [malloc_var_length];memset(best_array, 0, sizeof(int) * malloc_var_length);
    temp_lit = new int [malloc_var_length];memset(temp_lit, 0, sizeof(int) * malloc_var_length);
}

void Satlike::free_memory()
{
    int malloc_var_length = num_vars+10;
    int malloc_clause_length = num_clauses+10;

    if (unit_clause != NULL){
        delete[] unit_clause;
        unit_clause = NULL;
    }

    if(var_lit != NULL){
        for(int i=0; i< malloc_var_length; ++i){
            if (var_lit[i] != NULL){
                delete[] var_lit[i];
            }
        }
        delete[] var_lit;
        var_lit = NULL;
    }

    if(var_lit_count != NULL){
        delete [] var_lit_count;
        var_lit_count = NULL;
    }

    if(clause_lit != NULL){
        for(int i=0; i< malloc_clause_length; ++i){
            if (clause_lit[i] != NULL){
                delete[] clause_lit[i];
            }
        }
        delete[] clause_lit;
        clause_lit = NULL;
    }

    if(clause_lit_count != NULL){
        delete[] clause_lit_count;
        clause_lit_count = NULL;
    }

    if(score != NULL){
       delete [] score;
       score = NULL;
    }

    if(var_neighbor != NULL){
        for(int i=0; i< malloc_var_length; ++i){
            if (var_neighbor[i] != NULL){
                delete[] var_neighbor[i];
            }
        }
        delete [] var_neighbor;
        var_neighbor = NULL;
    }

    if(var_neighbor_count != NULL){
        delete [] var_neighbor_count;
        var_neighbor_count = NULL;
    }

    if(time_stamp != NULL){
        delete [] time_stamp;
        time_stamp = NULL;
    }

    if(neighbor_flag != NULL){
        delete [] neighbor_flag;
        neighbor_flag = NULL;
    }

    if(temp_neighbor != NULL){
        delete [] temp_neighbor;
        temp_neighbor = NULL;
    }

    if(org_clause_weight != NULL){
        delete [] org_clause_weight;
        org_clause_weight = NULL;
    }

    if(clause_weight != NULL){
        delete [] clause_weight;
        clause_weight = NULL;
    }

    if(sat_count != NULL){
        delete [] sat_count;
        sat_count = NULL;
    }

    if(sat_var != NULL){
        delete [] sat_var;
        sat_var = NULL;
    }

    if(clause_selected_count != NULL){
        delete [] clause_selected_count;
        clause_selected_count = NULL;
    }

    if(best_soft_clause != NULL){
        delete [] best_soft_clause;
        best_soft_clause = NULL;
    }

    if(hardunsat_stack != NULL){
        delete [] hardunsat_stack;
        hardunsat_stack = NULL;
    }

    if(index_in_hardunsat_stack != NULL){
        delete [] index_in_hardunsat_stack;
        index_in_hardunsat_stack = NULL;
    }

    if(softunsat_stack != NULL){
        delete [] softunsat_stack;
        softunsat_stack = NULL;
    }

    if(index_in_softunsat_stack != NULL){
        delete [] index_in_softunsat_stack;
        index_in_softunsat_stack = NULL;
    }

    if(unsatvar_stack != NULL){
        delete [] unsatvar_stack;
        unsatvar_stack = NULL;
    }

    if(index_in_unsatvar_stack != NULL){
        delete [] index_in_unsatvar_stack;
        index_in_unsatvar_stack = NULL;
    }

    if(unsat_app_count != NULL){
        delete [] unsat_app_count;
        unsat_app_count = NULL;
    }

    if(goodvar_stack != NULL){
        delete [] goodvar_stack;
        goodvar_stack = NULL;
    }

    if(already_in_goodvar_stack != NULL){
        delete [] already_in_goodvar_stack;
        already_in_goodvar_stack = NULL;
    }

    //delete [] fix;

    if(cur_soln != NULL){
        delete [] cur_soln;
        cur_soln = NULL;
    }

    if(best_soln != NULL){
        delete [] best_soln;
        best_soln = NULL;
    }

    if(local_opt_soln != NULL){
        delete [] local_opt_soln;
        local_opt_soln = NULL;
    }

    if(large_weight_clauses != NULL){
        delete [] large_weight_clauses;
        large_weight_clauses = NULL;
    }

    if(soft_large_weight_clauses != NULL){
        delete [] soft_large_weight_clauses;
        soft_large_weight_clauses = NULL;
    }

    if(already_in_soft_large_weight_stack != NULL){
        delete [] already_in_soft_large_weight_stack;
        already_in_soft_large_weight_stack = NULL;
    }

    if(best_array != NULL){
        delete [] best_array;
        best_array = NULL;
    }

    if(temp_lit != NULL){
        delete [] temp_lit;
        temp_lit = NULL;
    }

    //Shaswata
    if (deci != NULL){
        delete deci;
        deci = NULL;
    }
    /*if (dimParser != NULL){
        delete dimParser;
        dimParser = NULL;
    }
    if (finalFormattedResult != NULL){
        delete[] finalFormattedResult;
        finalFormattedResult = NULL;
    }*/

    if (generator != NULL){
        delete generator;
        generator = NULL;
    }

}

void Satlike::build_neighbor_relation()
{
    int	i,j,count;
    int v,c,n;
    int temp_neighbor_count;

    for(v=1; v<=num_vars; ++v)
    {
        neighbor_flag[v] = 1;
        temp_neighbor_count = 0;

        for(i=0; i<var_lit_count[v]; ++i)
        {
            c = var_lit[v][i].clause_num;
            for(j=0; j<clause_lit_count[c]; ++j)
            {
                n=clause_lit[c][j].var_num;
                if(neighbor_flag[n]!=1)
                {
                    neighbor_flag[n] = 1;
                    temp_neighbor[temp_neighbor_count++] = n;
                }
            }
        }

        neighbor_flag[v] = 0;

        var_neighbor[v] = new int[temp_neighbor_count];
        memset(var_neighbor[v], 0, sizeof(int) * temp_neighbor_count);
        var_neighbor_count[v]=temp_neighbor_count;

        count = 0;
        for(i=0; i<temp_neighbor_count; i++)
        {
            var_neighbor[v][count++] = temp_neighbor[i];
            neighbor_flag[temp_neighbor[i]] = 0;
        }
    }
}

void Satlike::build_instance(const char *filename)
{
    //Shaswata: Moved all the initialization of constructor here
    //due to from python interface same object is being resued which doesn't go via constructor
    INITIAL_MAX_FLIP = 10000000;

    problem_weighted=1;
    partial=1; //1 if the instance has hard clauses, and 0 otherwise.

    max_clause_length=0;
    min_clause_length=100000000;

    //size of the instance
    num_vars=0;     //var index from 1 to num_vars
    num_clauses=0;      //clause index from 0 to num_clauses-1
    num_hclauses=0;
    num_sclauses=0;

    print_time=240;
    cutoff_time=300;

    //finalFormattedResult = NULL;
    //dimParser = NULL;
    deci = NULL;
    generator = NULL;
    top_clause_weight = 0;
    total_soft_weight = 0;
    soft_unsat_weight = 0;
    opt_unsat_weight = 0;

    //======================================

    istringstream iss;
    char    line[1024 + 1]; //Shaswata - 1 byte additional for '\0'
    string  line2;
    char    tempstr1[10];
    char    tempstr2[10];
    int     cur_lit;
    int     i,v,c;
    //int     temp_lit[MAX_VARS];

    assert(filename != NULL); //Shaswata

    ifstream infile(filename);
    //if(infile==NULL) //modification done by Shaswata
    if(!infile.good())
    {
        cout<<"c the input filename "<<filename<<" is invalid, please input the correct filename."<<endl<<flush;
        exit(-1);
    }

    /*** build problem data structures of the instance ***/

    getline(infile,line2);

    while(line2[0]!='p')
    {
        getline(infile,line2);
    }

    assert(line2.length() <= (sizeof(line)-1));//Shaswata
    for(i=0;i<line2.length();i++)
    {
        line[i]=line2[i];
    }
    line[line2.length()] = '\0'; //Shaswata

    int read_items=sscanf(line, "%s %s %d %d %lld", tempstr1, tempstr2, &num_vars, &num_clauses, &top_clause_weight);

    if(read_items<5)
    {
        if(strcmp(tempstr2,"cnf")==0)
        {
            problem_weighted=0;
        }

        partial = 0;
        top_clause_weight=-1;
    }

    allocate_memory();

    for (c = 0; c < num_clauses; c++)
    {
        clause_lit_count[c] = 0;
        clause_lit[i]=NULL;
    }
    for (v=1; v<=num_vars; ++v)
    {
        var_lit_count[v] = 0;
        var_lit[v]=NULL;
        var_neighbor[v]=NULL;
    }

    num_hclauses=num_sclauses=0;
    unit_clause_count=0;
    //Now, read the clauses, one at a time.
    int lit_redundent,clause_redundent;

    c=0;
    while(getline(infile,line2))
    {
        if( c%5000==0 )
        {
            if(get_runtime()>30)
            {
                string inputfile_string=filename;
                string my_command="./Open-LinSBPS_static -algorithm=1 -phase-saving-solution-based -eproc -eprocfactor=5 -eprocthreshold=500000 -end-print=1 "+inputfile_string;
                int system_item=system(my_command.c_str());
            }
        }
        if(line2[0]=='c') continue;
        else
        {
            iss.clear();
            iss.str(line2);
            iss.seekg(0,ios::beg);
        }
        clause_redundent=0;
        clause_lit_count[c] = 0;

        if (problem_weighted!=0) 
            iss>>org_clause_weight[c];
        else org_clause_weight[c]=1;

        if (org_clause_weight[c]!=top_clause_weight) 
        {
            total_soft_weight += org_clause_weight[c];
            num_sclauses++;
        }
        else num_hclauses++;

        iss>>cur_lit;
        while (cur_lit != 0) {
            /*
			lit_redundent=0;
			for(int p=0; p<clause_lit_count[c]; p++)
			{
				if(cur_lit==temp_lit[p]){
					lit_redundent=1;
					break;
				}
				else if(cur_lit==-temp_lit[p]){
					clause_redundent=1;
					break;
				}
			}

			if(lit_redundent==0)
			{*/
            temp_lit[clause_lit_count[c]] = cur_lit;
            clause_lit_count[c]++;
            //}

            iss>>cur_lit;
        }

        if(clause_redundent==0) //the clause is not tautology
        {
            clause_lit[c] = new lit[clause_lit_count[c]+1];
            memset(clause_lit[c], 0, sizeof(lit) * (clause_lit_count[c]+1));
            for(i=0; i<clause_lit_count[c]; ++i)
            {
                clause_lit[c][i].clause_num = c;
                clause_lit[c][i].var_num = abs(temp_lit[i]);

                if (temp_lit[i] > 0) clause_lit[c][i].sense = 1;
                else clause_lit[c][i].sense = 0;

                var_lit_count[clause_lit[c][i].var_num]++;
            }
            clause_lit[c][i].var_num=0; 
            clause_lit[c][i].clause_num = -1;

            if(clause_lit_count[c]==1)
            {
                unit_clause[unit_clause_count++]=clause_lit[c][0];
            }

            if(clause_lit_count[c]>max_clause_length) max_clause_length=clause_lit_count[c];
            if(clause_lit_count[c]<min_clause_length) min_clause_length=clause_lit_count[c];

            c++;
        }
        else
        {
            num_clauses--;
            clause_lit_count[c] = 0;
        }
    }

    infile.close();


    //creat var literal arrays
    for (v=1; v<=num_vars; ++v)
    {
        var_lit[v] = new lit[var_lit_count[v]+1];
        memset(var_lit[v], 0, sizeof(lit) * (var_lit_count[v]+1));
        var_lit_count[v] = 0;	//reset to 0, for build up the array
    }
    //scan all clauses to build up var literal arrays
    for (c = 0; c < num_clauses; ++c)
    {
        for(i=0; i<clause_lit_count[c]; ++i)
        {
            v = clause_lit[c][i].var_num;
            var_lit[v][var_lit_count[v]] = clause_lit[c][i];
            ++var_lit_count[v];
        }
    }
    for (v=1; v<=num_vars; ++v)
        var_lit[v][var_lit_count[v]].clause_num=-1;


    build_neighbor_relation();

    //Shaswata: following three variables have to initialized for every run of algo
    best_soln_feasible=0;
    feasible_flag=0;
    opt_unsat_weight=total_soft_weight+1;
}

void Satlike::init(vector<int>& i_solution, bool override_init_sol)
{
    int 		v,c;
    int			i,j;
    hard_unsat_nb=0;
    soft_unsat_weight=0;
    local_soln_feasible=0;
    local_opt_unsat_weight=top_clause_weight+1;
    soft_large_weight_clauses_count=0;

    if(best_soln_feasible==0)
    {
        for (c = 0; c<num_clauses; c++)
        {
            already_in_soft_large_weight_stack[c]=0;
            clause_selected_count[c]=0;

            if (org_clause_weight[c]==top_clause_weight)
                clause_weight[c] = 1;
            else
            {
                clause_weight[c] = 0;
            }
        }
    }
    else
    {
        //Initialize clause information
        for (c = 0; c<num_clauses; c++)
        {
            already_in_soft_large_weight_stack[c]=0;
            clause_selected_count[c]=0;

            if (org_clause_weight[c]==top_clause_weight)
                clause_weight[c] = 1;
            else
            {
                clause_weight[c] = org_clause_weight[c];
                if(clause_weight[c]>1 && already_in_soft_large_weight_stack[c]==0)
                {
                    already_in_soft_large_weight_stack[c]=1;
                    soft_large_weight_clauses[soft_large_weight_clauses_count++] = c;
                }
            }
        }
    }
    //init solution
    if((feasible_flag==1) && !override_init_sol) //Shaswata - included override_init_sol
    {
        for (v = 1; v <= num_vars; v++)
        {
            cur_soln[v]=best_soln[v];
            time_stamp[v] = 0;
            unsat_app_count[v] = 0;
        }
        feasible_flag=2;
    }
    else if((i_solution.size()==0) && !override_init_sol)
    {
        for (v = 1; v <= num_vars; v++) 
        {
            cur_soln[v]=0;
            time_stamp[v] = 0;
            unsat_app_count[v] = 0;
        }
    }
    else
    {
        for (v = 1; v <= num_vars; v++) 
        {
            cur_soln[v]=i_solution[v];
            time_stamp[v] = 0;
            unsat_app_count[v] = 0;
        }
    }

    //init stacks
    hardunsat_stack_fill_pointer = 0;
    softunsat_stack_fill_pointer = 0;
    unsatvar_stack_fill_pointer = 0;
    large_weight_clauses_count=0;


    /* figure out sat_count, sat_var and init unsat_stack */
    for (c=0; c<num_clauses; ++c)
    {
        sat_count[c] = 0;
        for(j=0; j<clause_lit_count[c]; ++j)
        {
            if (cur_soln[clause_lit[c][j].var_num] == clause_lit[c][j].sense)
            {
                sat_count[c]++;
                sat_var[c] = clause_lit[c][j].var_num;
            }
        }
        if (sat_count[c] == 0)
        {
            unsat(c);
        }
    }

    /*figure out score*/
    for (v=1; v<=num_vars; v++)
    {
        score[v]=0;
        for(i=0; i<var_lit_count[v]; ++i)
        {
            c = var_lit[v][i].clause_num;
            if (sat_count[c]==0) score[v]+=clause_weight[c];
            else if (sat_count[c]==1 && var_lit[v][i].sense==cur_soln[v]) score[v]-=clause_weight[c];
        }
    }

    //init goodvars stack
    goodvar_stack_fill_pointer = 0;
    for (v=1; v<=num_vars; v++)
    {
        if(score[v]>0)
        {
            already_in_goodvar_stack[v] = goodvar_stack_fill_pointer;
            mypush(v,goodvar_stack);
        }
        else
            already_in_goodvar_stack[v] = -1;
    }

    max_flips=INITIAL_MAX_FLIP; //Shaswata - it was not reinited
}

int Satlike::pick_var()
{
    int     i,v;
    int     best_var;

    if(goodvar_stack_fill_pointer>0 )
    {
        if( (my_get_rand()%MY_RAND_MAX_INT)*BASIC_SCALE< rdprob )
            return goodvar_stack[my_get_rand()%goodvar_stack_fill_pointer];

        if (goodvar_stack_fill_pointer < hd_count_threshold)
        {
            best_var = goodvar_stack[0];
            for (i=1; i<goodvar_stack_fill_pointer; ++i)
            {
                v = goodvar_stack[i];
                if (score[v]>score[best_var]) best_var=v;
                else if (score[v]==score[best_var])
                {
                    if(time_stamp[v]<time_stamp[best_var]) best_var=v;
                }
            }
            return best_var;
        }
        else
        {
            best_var = goodvar_stack[my_get_rand()%goodvar_stack_fill_pointer];
            for (i=1; i<hd_count_threshold; ++i)
            {
                v = goodvar_stack[my_get_rand()%goodvar_stack_fill_pointer];
                if (score[v]>score[best_var]) best_var=v;
                else if (score[v]==score[best_var])
                {
                    if(time_stamp[v]<time_stamp[best_var]) best_var=v;
                }
            }
            return best_var;
        }
    }

    update_clause_weights();

    int sel_c;    
    lit* p;

    if (hardunsat_stack_fill_pointer>0) 
    {
        sel_c = hardunsat_stack[my_get_rand()%hardunsat_stack_fill_pointer];
    }
    else 
    {
        sel_c= softunsat_stack[my_get_rand()%softunsat_stack_fill_pointer];
    }
    if( (my_get_rand()%MY_RAND_MAX_INT)*BASIC_SCALE< rwprob )
        return clause_lit[sel_c][my_get_rand()%clause_lit_count[sel_c]].var_num;

    best_var = clause_lit[sel_c][0].var_num;
    p=clause_lit[sel_c];
    for(p++; (v=p->var_num)!=0; p++)
    {           
        if (score[v]>score[best_var]) best_var=v;
        else if (score[v]==score[best_var])
        {
            if(time_stamp[v]<time_stamp[best_var]) best_var=v;
        }
    }
    return best_var;
}

void Satlike::update_goodvarstack1(int flipvar)
{
    int v;
    //remove the vars no longer goodvar in goodvar stack
    for(int index=goodvar_stack_fill_pointer-1; index>=0; index--)
    {
        v = goodvar_stack[index];
        if(score[v]<=0)
        {
            goodvar_stack[index] = mypop(goodvar_stack);
            already_in_goodvar_stack[v] = -1;
        }
    }

    //add goodvar
    for(int i=0; i<var_neighbor_count[flipvar]; ++i)
    {
        v = var_neighbor[flipvar][i];
        if( score[v] > 0)
        {
            if(already_in_goodvar_stack[v] == -1)
            {
                already_in_goodvar_stack[v] = goodvar_stack_fill_pointer;
                mypush(v,goodvar_stack);
            }
        }
    }
}
void Satlike::update_goodvarstack2(int flipvar)
{
    if(score[flipvar]>0 && already_in_goodvar_stack[flipvar]==-1)
    {
        already_in_goodvar_stack[flipvar]=goodvar_stack_fill_pointer;
        mypush(flipvar,goodvar_stack);
    }
    else if(score[flipvar]<=0 && already_in_goodvar_stack[flipvar]!=-1)
    {
        int index=already_in_goodvar_stack[flipvar];
        int last_v=mypop(goodvar_stack);
        goodvar_stack[index]=last_v;
        already_in_goodvar_stack[last_v]=index;
        already_in_goodvar_stack[flipvar]=-1;
    }
    int i,v;
    for(i=0; i<var_neighbor_count[flipvar]; ++i)
    {
        v = var_neighbor[flipvar][i];
        if( score[v] > 0)
        {
            if(already_in_goodvar_stack[v] == -1)
            {
                already_in_goodvar_stack[v] = goodvar_stack_fill_pointer;
                mypush(v,goodvar_stack);
            }
        }
        else if(already_in_goodvar_stack[v]!=-1)
        {
            int index=already_in_goodvar_stack[v];
            int last_v=mypop(goodvar_stack);
            goodvar_stack[index]=last_v;
            already_in_goodvar_stack[last_v]=index;
            already_in_goodvar_stack[v]=-1;
        }
    }
}

void Satlike::flip(int flipvar)
{
    int i,v,c;
    int index;
    lit* clause_c;

    int	org_flipvar_score = score[flipvar];
    cur_soln[flipvar] = 1 - cur_soln[flipvar];

    for(i=0; i<var_lit_count[flipvar]; ++i)
    {
        c = var_lit[flipvar][i].clause_num;
        clause_c = clause_lit[c];

        if(cur_soln[flipvar] == var_lit[flipvar][i].sense)
        {
            ++sat_count[c];
            if (sat_count[c] == 2) //sat_count from 1 to 2
            {
                score[sat_var[c]] += clause_weight[c];
            }
            else if (sat_count[c] == 1) // sat_count from 0 to 1
            {
                sat_var[c] = flipvar;//record the only true lit's var
                for(lit* p=clause_c; (v=p->var_num)!=0; p++) 
                {
                    score[v] -= clause_weight[c];
                }
                sat(c);
            }
        }
        else // cur_soln[flipvar] != cur_lit.sense
        {
            --sat_count[c];
            if (sat_count[c] == 1) //sat_count from 2 to 1
            {
                for(lit* p=clause_c; (v=p->var_num)!=0; p++) 
                {
                    if(p->sense == cur_soln[v] )
                    {
                        score[v] -= clause_weight[c];
                        sat_var[c] = v;
                        break;
                    }
                }
            }
            else if (sat_count[c] == 0) //sat_count from 1 to 0
            {
                for(lit* p=clause_c; (v=p->var_num)!=0; p++)
                {
                    score[v] += clause_weight[c];
                }
                unsat(c);    
            }//end else if  
        }//end else
    }

    //update information of flipvar
    score[flipvar] = -org_flipvar_score;
    update_goodvarstack1(flipvar);
}

void Satlike::local_search(vector<int>& init_solution)
{
    settings();
    max_flips = 200000000;
    init(init_solution);
    cout<<"time is "<<get_runtime()<<endl;
    cout<<"hard unsat nb is "<<hard_unsat_nb<<endl;
    cout<<"soft unsat nb is "<<soft_unsat_weight<<endl;
    cout<<"goodvar nb is "<<goodvar_stack_fill_pointer<<endl;
}

void Satlike::print_best_solution(bool print_var_assign)
{
    //verify_sol();
    if(best_soln_feasible==0) return ;
    cout<<"o "<<opt_unsat_weight<<endl;

    if (!print_var_assign){
        return;
    }

    std::stringstream s;
    s<<"v";
    for(int i=1;i<=num_vars;++i)
    {
        s<<" ";
        if(best_soln[i]==0) s<<"-";
        s<<i;
    }
    printf("%s\n",s.str().c_str());
    cout << flush;
}

void Satlike::algo_init(unsigned int seed, bool debug)
{
    //Shaswata: moving from build_instance() to here so that same satlike instance can be used multiple times
    best_soln_feasible=0;
    feasible_flag=0;
    opt_unsat_weight=total_soft_weight+1;
    //===================================

    settings(debug);
    assert(generator == NULL);
    assert(deci == NULL);

    std::array<unsigned int,std::mt19937::state_size> myarray;
    myarray.fill(seed);
    std::seed_seq initial_state_of_mt19937 (myarray.cbegin(), myarray.cend());
    generator = new std::mt19937(initial_state_of_mt19937);//Shaswata

    deci = new Decimation(var_lit,var_lit_count,clause_lit,org_clause_weight,top_clause_weight,
                        *generator);
    deci->make_space(num_clauses,num_vars);
}

void Satlike::init_with_decimation_stepwise(bool through_decimation)
{
    if(through_decimation && (feasible_flag!=1))
    {
        deci->init(local_opt_soln,best_soln,unit_clause,unit_clause_count,clause_lit_count);

        deci->unit_prosess();

        init_solution.resize(num_vars+1);
        for(int i=1;i<=num_vars;++i)
        {
            init_solution[i]=deci->get_fix(i);
        }
    }
    init(init_solution, !through_decimation);
}

long long Satlike::local_search_stepwise(int t, float sp,  int hinc, int eta, int max_search,
        unsigned int current_step, bool adaptive_search_extent, int verbose)
{
    update_hyper_param(t, sp, hinc, eta, max_search);
    long long result = (hard_unsat_nb > 0) ? -hard_unsat_nb : opt_unsat_weight;
    if (hard_unsat_nb==0 && (soft_unsat_weight<opt_unsat_weight || best_soln_feasible==0) )
    {
        if(best_soln_feasible==0)
        {
            best_soln_feasible=1;
            opt_unsat_weight = soft_unsat_weight;
            result = opt_unsat_weight;//Shaswata, we need to update result as it is holding older opt_unsat_weight
            for(int v=1; v<=num_vars; ++v) best_soln[v] = cur_soln[v];
            feasible_flag=1;
            if(opt_unsat_weight==0){
                return (0);
            }
        }
        //if (soft_unsat_weight<top_clause_weight) //Shaswata - experimental
        else if (soft_unsat_weight<opt_unsat_weight)
        {
            best_soln_feasible=1;
            opt_unsat_weight = soft_unsat_weight;
            result = opt_unsat_weight;//Shaswata, we need to update result as it is holding older opt_unsat_weight
            for(int v=1; v<=num_vars; ++v) best_soln[v] = cur_soln[v];

            if(opt_unsat_weight==0){
                return (0);
            }
        }
    }

    if(hard_unsat_nb==0 && (soft_unsat_weight<local_opt_unsat_weight || local_soln_feasible==0))
    {
        if(soft_unsat_weight<top_clause_weight)
        {
            local_soln_feasible=1;
            local_opt_unsat_weight=soft_unsat_weight;
            if(adaptive_search_extent){
                max_flips=current_step+max_non_improve_flip;
            }
            if(verbose > 1) cout << "c changing max_flips=" << max_flips << " soft_unsat_weight=" << soft_unsat_weight << endl;
        }
    }

    prev_hard_unsat_nb = hard_unsat_nb;
    prev_soft_unsat_weight = soft_unsat_weight;

    int flipvar = pick_var();
    fliped_var = flipvar;
    flip(flipvar);
    time_stamp[flipvar] = current_step;
    return (result);
}

void Satlike::local_search_with_decimation_using_steps(unsigned int seed, int maxTimeToRunInSec,
        int t, float sp,  int hinc, int eta, int max_search,
        bool adaptive_search_extent, int verbose_level, bool verification_to_be_done)
{
    long long iteration_count = 0;
    algo_init(seed, verbose_level > 0);
    long long last_soft_unsat_weight = get_total_soft_weight()+1;
    for(int tries=1;tries<max_tries;++tries)
    {
        init_with_decimation_stepwise();

        for (unsigned int current_step = 1; current_step<get_max_flips(); ++current_step)
        {
            ++iteration_count;
            long long result = local_search_stepwise(
                    (t == -1) ? hd_count_threshold : t,
                    (sp < 0) ? smooth_probability : sp,
                    (hinc == -1) ? h_inc: hinc,
                    (eta == -1) ? softclause_weight_threshold: eta,
                    (max_search == -1)? max_non_improve_flip: max_search,
                    current_step,
                    adaptive_search_extent,
                    verbose_level);

            if ((result >= 0) && (result < last_soft_unsat_weight)) {
                if(verbose_level > 0){
                    float fractSat = (float)(get_total_soft_weight() - result) / (float)get_total_soft_weight();
                    cout << "c iteration_count=" << iteration_count << " tries=" << tries << " current_step=" << current_step <<
                            " opt_unsat_weight=" << result <<
                            " fractSat =" << fractSat << " time-took=" << get_runtime()
                            << endl;
                }
                if(verification_to_be_done) assert(verify_sol() == 1);
                last_soft_unsat_weight = result;
            }

            if (0 == get_opt_unsat_weight()){
                return;
            }

            if(get_runtime() > maxTimeToRunInSec){
                if (verbose_level > 0) cout<<"c time exhausted iteration_count=" << iteration_count << endl << flush;
                return;
            }
        }

        if(verbose_level > 0) cout<<"c Episode completed iteration_count=" << iteration_count << endl << flush;
    }
}

void Satlike::local_search_with_decimation(unsigned int seed, vector<int>& i_solution, const char* inputfile, int max_time_to_run,
        int verbose, bool verification_to_be_done)
{
    settings(verbose > 0);

    assert(generator == NULL);

    std::array<unsigned int,std::mt19937::state_size> myarray;
    myarray.fill(seed);
    std::seed_seq initial_state_of_mt19937 (myarray.cbegin(), myarray.cend());
    generator = new std::mt19937(initial_state_of_mt19937);//Shaswata

    Decimation l_deci(var_lit,var_lit_count,clause_lit,org_clause_weight,top_clause_weight, *generator);
    l_deci.make_space(num_clauses,num_vars);
    long long iteration_count = 0;
    int num_of_unsuccessful_consecutive_try = 0;//Shaswata - experimental
    for(int tries=1;tries<max_tries;++tries)
    {
        if(verbose > 1) cout<<"c iteration_count=" << iteration_count << " try=" << tries << " num_of_unsuccessful_consecutive_try=" << num_of_unsuccessful_consecutive_try << endl << flush;

        //Shaswata: note that initially feasible_flag=0, after that it transitions to 1,
        //when first valid solution found. Then it will be transitioned to 2
        if(feasible_flag!=1)
        {
            if(verbose > 1) cout<<"c decimation process re-inited feasible_flag="<<feasible_flag << endl;
            l_deci.init(local_opt_soln,best_soln,unit_clause,unit_clause_count,clause_lit_count);

            l_deci.unit_prosess();

            i_solution.resize(num_vars+1);
            for(int i=1;i<=num_vars;++i)
            {
                i_solution[i]=l_deci.get_fix(i);
            }
        }

        init(i_solution);
        for (unsigned int step = 1; step<max_flips; ++step)
        {
            ++iteration_count;
            if (hard_unsat_nb==0 && (soft_unsat_weight<opt_unsat_weight || best_soln_feasible==0) )
            {
                if(best_soln_feasible==0)
                {
                    best_soln_feasible=1;
                    opt_unsat_weight = soft_unsat_weight;
                    if(verbose > 0) cout<<"c iteration_count=" << iteration_count << " try="<< tries << " step="<< step << " opt-wt="<<opt_unsat_weight<< " time-took=" << get_runtime() << endl;
                    for(int v=1; v<=num_vars; ++v) best_soln[v] = cur_soln[v];
                    if(verification_to_be_done) assert(verify_sol() == 1);
                    feasible_flag=1;
                    num_of_unsuccessful_consecutive_try=-1;
                    if(opt_unsat_weight==0){
                        return;
                    }
                    //break; //Shaswata - experimental
                }
                //if (soft_unsat_weight<top_clause_weight) //Shaswata - experimental
                else if (soft_unsat_weight<opt_unsat_weight)
                {
                    best_soln_feasible=1;
                    opt_unsat_weight = soft_unsat_weight;
                    if(verbose > 0) cout<<"c iteration_count=" << iteration_count << " try="<< tries << " step="<< step << " opt-wt="<<opt_unsat_weight<< " time-took=" << get_runtime() << endl;
                    for(int v=1; v<=num_vars; ++v) best_soln[v] = cur_soln[v];
                    if(verification_to_be_done) assert(verify_sol() == 1);
                    num_of_unsuccessful_consecutive_try=-1;
                    if(opt_unsat_weight==0){
                        return;
                    }
                }
            }

            if(hard_unsat_nb==0 && (soft_unsat_weight<local_opt_unsat_weight || local_soln_feasible==0))
            {
                if(soft_unsat_weight<top_clause_weight)
                {
                    //Shaswata: note that max-flip can be tried to increase even when soft_unsat_weight
                    //may not be better than opt_unsat_weight
                    local_soln_feasible=1;
                    local_opt_unsat_weight=soft_unsat_weight;
                    max_flips=step+max_non_improve_flip;
                    if(verbose > 1) cout << "c changing max_flips=" << max_flips << " soft_unsat_weight=" << soft_unsat_weight << endl;
                }
            }
            if (step%1000==0)
            {
                double elapse_time=get_runtime();
                if(best_soln_feasible==0 && elapse_time>40)
                {
                    string inputfile_string=inputfile;
                    string my_command="./Open-LinSBPS_static -algorithm=1 -phase-saving-solution-based -eproc -eprocfactor=5 -eprocthreshold=500000 -end-print=1 "+inputfile_string;
                    int system_item=system(my_command.c_str());
                }
            }
            int flipvar = pick_var();
            flip(flipvar);
            time_stamp[flipvar] = step;

            if(get_runtime() > max_time_to_run){
                if(verbose > 0) cout<<"c time exhausted iteration_count=" << iteration_count << endl << flush;
                return;
            }
        }
        num_of_unsuccessful_consecutive_try ++;
        if(verbose > 0) cout<<"c Episode completed iteration_count=" << iteration_count << endl << flush;
    }
}

bool Satlike::verify_sol()
{        
    int c,j,flag;
    long long verify_unsat_weight=0;

    for (c = 0; c<num_clauses; ++c)
    {
        flag = 0;
        for(j=0; j<clause_lit_count[c]; ++j){
            if (best_soln[clause_lit[c][j].var_num] == clause_lit[c][j].sense) {
                flag = 1;
                break;
            }
        }

        if(flag ==0)
        {
            if(org_clause_weight[c]==top_clause_weight)//verify hard clauses
            {
                //output the clause unsatisfied by the solution
                cout<<"c Error: hard clause "<<c<<" is not satisfied"<<endl;

                cout<<"c ";
                for(j=0; j<clause_lit_count[c]; ++j)
                {
                    if(clause_lit[c][j].sense==0)cout<<"-";
                    cout<<clause_lit[c][j].var_num<<" ";
                }
                cout<<endl;
                cout<<"c ";
                for(j=0; j<clause_lit_count[c]; ++j)
                    cout<<best_soln[clause_lit[c][j].var_num]<<" ";
                cout<<endl;
                return 0;
            }
            else
            {
                verify_unsat_weight+=org_clause_weight[c];
            }
        }
    }

    if(verify_unsat_weight==opt_unsat_weight)  return 1;
    else{
        cout<<"c Error: find opt="<<opt_unsat_weight<<", but verified opt="<<verify_unsat_weight<<endl;
    }
    return 0;
}

void Satlike::simple_print()
{
    if(best_soln_feasible==1)
    {
        if(verify_sol()==1)
            cout<<opt_unsat_weight<<'\t'<<opt_time<<endl;
        else
            cout<<"solution is wrong "<<endl;
    }
    else
        cout<<-1<<'\t'<<-1<<endl;
}

void Satlike::increase_weights()
{
    int i,c,v;
    for(i=0; i < hardunsat_stack_fill_pointer; ++i)
    {
        c=hardunsat_stack[i];
        clause_weight[c]+=h_inc;

        if(clause_weight[c] == (h_inc+1))
            large_weight_clauses[large_weight_clauses_count++] = c;

        for(lit* p=clause_lit[c]; (v=p->var_num)!=0; p++)
        {
            score[v]+=h_inc;
            if (score[v]>0 && already_in_goodvar_stack[v]==-1)           
            {
                already_in_goodvar_stack[v] = goodvar_stack_fill_pointer;
                mypush(v,goodvar_stack);
            }
        }
    }
    for(i=0; i < softunsat_stack_fill_pointer; ++i)
    {
        c=softunsat_stack[i];
        if(clause_weight[c]>softclause_weight_threshold) continue;
        else clause_weight[c]++;

        if(clause_weight[c]>1 && already_in_soft_large_weight_stack[c]==0)
        {
            already_in_soft_large_weight_stack[c]=1;
            soft_large_weight_clauses[soft_large_weight_clauses_count++] = c;
        }
        for(lit* p=clause_lit[c]; (v=p->var_num)!=0; p++)
        {
            score[v]++;
            if (score[v]>0 && already_in_goodvar_stack[v]==-1)           
            {
                already_in_goodvar_stack[v] = goodvar_stack_fill_pointer;
                mypush(v,goodvar_stack);
            }
        }
    }
}

void Satlike::smooth_weights()
{
    int i, clause, v;

    for(i=0; i<large_weight_clauses_count; i++)
    {
        clause = large_weight_clauses[i];
        if(sat_count[clause]>0)
        {
            clause_weight[clause]-=h_inc;

            if(clause_weight[clause]==1)
            {
                large_weight_clauses[i] = large_weight_clauses[--large_weight_clauses_count];
                i--;
            }
            if(sat_count[clause] == 1)
            {
                v = sat_var[clause];
                score[v]+=h_inc;
                if (score[v]>0 && already_in_goodvar_stack[v]==-1)           
                {
                    already_in_goodvar_stack[v] = goodvar_stack_fill_pointer;
                    mypush(v,goodvar_stack);
                }
            }
        }
    }

    for(i=0; i<soft_large_weight_clauses_count; i++)
    {
        clause = soft_large_weight_clauses[i];
        if(sat_count[clause]>0)
        {
            clause_weight[clause]--;
            if(clause_weight[clause]==1 && already_in_soft_large_weight_stack[clause]==1)
            {
                already_in_soft_large_weight_stack[clause]=0;
                soft_large_weight_clauses[i] = soft_large_weight_clauses[--soft_large_weight_clauses_count];
                i--;
            }
            if(sat_count[clause] == 1)
            {
                v = sat_var[clause];
                score[v]++;
                if (score[v]>0 && already_in_goodvar_stack[v]==-1)           
                {
                    already_in_goodvar_stack[v] = goodvar_stack_fill_pointer;
                    mypush(v,goodvar_stack);
                }
            }
        }
    }
}

void Satlike::update_clause_weights()
{	
    if( ((my_get_rand()%MY_RAND_MAX_INT)*BASIC_SCALE)<smooth_probability
            && large_weight_clauses_count>large_clause_count_threshold  )
    {
        smooth_weights();
    }
    else
    {
        increase_weights();
    }
}

inline void Satlike::unsat(int clause)
{
    if(org_clause_weight[clause]==top_clause_weight) 
    {
        index_in_hardunsat_stack[clause] = hardunsat_stack_fill_pointer;
        mypush(clause,hardunsat_stack);
        hard_unsat_nb++;
    }
    else 
    {
        index_in_softunsat_stack[clause] = softunsat_stack_fill_pointer;
        mypush(clause,softunsat_stack);
        soft_unsat_weight += org_clause_weight[clause];
    }   
}

inline void Satlike::sat(int clause)
{
    int index,last_unsat_clause;

    if (org_clause_weight[clause]==top_clause_weight) 
    {
        last_unsat_clause = mypop(hardunsat_stack);
        index = index_in_hardunsat_stack[clause];
        hardunsat_stack[index] = last_unsat_clause;
        index_in_hardunsat_stack[last_unsat_clause] = index;

        hard_unsat_nb--;
    }
    else 
    {
        last_unsat_clause = mypop(softunsat_stack);
        index = index_in_softunsat_stack[clause];
        softunsat_stack[index] = last_unsat_clause;
        index_in_softunsat_stack[last_unsat_clause] = index;

        soft_unsat_weight -= org_clause_weight[clause];
    }
}

//============================== Shaswata ================================
//Additional enhancement
//========================================================================
/*
static unsigned long long findUnsatClauseCount(const int* finalFormattedResult, const vector<CNFClause>& originalClauses,
        set<int>& unsatClauses, int debug) {
    unsigned long long nonSatClauseWeight = 0;
    if(debug >= 2){
        cout << "c list of unsat soft-clauses:" << std::endl;
    }
    int totalClauseSize = originalClauses.size();

    for (int clauseIndex = 0; clauseIndex < totalClauseSize; clauseIndex ++) {
        set<int>::const_iterator s;
        bool atleastOneVarTrue = false;
        const set<int>& vars = originalClauses[clauseIndex].getVars();
        for(s = vars.cbegin(); s != vars.cend(); s++){
            int var = *s;
            assert(var != 0);
            if (var > 0) {
                if (finalFormattedResult[var] == var) {
                    atleastOneVarTrue = true;
                    break;
                }
            } else {
                if (finalFormattedResult[-var] == var) {
                    atleastOneVarTrue = true;
                    break;
                }
            }
        }
        if (!atleastOneVarTrue) {
            if(debug >= 2){
                cout << originalClauses[clauseIndex];
            }
            assert(!(originalClauses[clauseIndex]).isHard());//can be only soft
            unsatClauses.insert(originalClauses[clauseIndex].getUniqueID());
            nonSatClauseWeight += originalClauses[clauseIndex].getWeight();
        }
    }

    if(debug){
        cout <<"c nonSatClauseWeight = " << nonSatClauseWeight << " count of unsat-clauses = " << unsatClauses.size() << std::endl;
        cout << std::flush;
    }
    return (nonSatClauseWeight);
}


void Satlike::addOnlyHardClausesToSolver(Solver& solver){

    const vector<CNFClause>& originalClauses = dimParser->getOriginalClauses();
    int originalClauseLen = originalClauses.size();
    for(int i = 0; i < originalClauseLen; i++){

        if(!originalClauses[i].isHard()){
            continue;
        }

        set<int>::const_iterator clauseVarIt;
        vec<Lit> c;
        const set<int>& listOfLits = originalClauses[i].getVars();
        for(clauseVarIt = listOfLits.cbegin(); clauseVarIt != listOfLits.cend(); clauseVarIt ++){
            //Reference: readClause() function in core/Dimacs.h
            int var = abs(*clauseVarIt)-1;
            while (var >= solver.nVars()) solver.newVar();
            c.push( (*clauseVarIt > 0) ? mkLit(var) : ~mkLit(var) );
        }

        solver.addClause(c);
    }
}

void Satlike::get_initial_search_space_using_solver(const char* inputfile, int verbose_level){

    if(dimParser == NULL){
        FILE* fp=fopen(inputfile, "r");
        assert(fp != NULL);
        dimParser = new WeightedDimacsParser(true);
        dimParser->parse(fp, false);
        assert(dimParser->getOriginalClauseCount() == dimParser->getOriginalClauses().size());
        assert((dimParser->getHardClauseCount() + dimParser->getSoftClauseCount()) == dimParser->getOriginalClauseCount());
        fclose(fp);
        finalFormattedResult = new int[dimParser->getOriginalVarCount() + 1];

        if (verbose_level > 0){
            cout << "c original clause count = " << dimParser->getOriginalClauseCount()
                                                         << " var = " << dimParser->getOriginalVarCount()
                                                         << " hard = " << dimParser->getHardClauseCount()
                                                         << " soft = " << dimParser->getSoftClauseCount()
                                                         << std::endl << std::flush;
        }
    }

    dimParser->randomShuffleClauses();

    try {
        Solver minisatSolver;

        addOnlyHardClausesToSolver( minisatSolver);
        if (verbose_level > 0){
            cout << "c solver clause count = " << minisatSolver.nClauses() << " var = " << minisatSolver.nVars() << endl << flush;
        }
        assert(dimParser->getOriginalVarCount() == minisatSolver.nVars());

        bool ret = minisatSolver.solve();
        assert(ret == true);

        memset(finalFormattedResult, 0, (dimParser->getOriginalVarCount() + 1) * sizeof(int));

        for (int i = 0; i < dimParser->getOriginalVarCount(); i++) {
            assert(minisatSolver.model[i] != l_Undef);
            finalFormattedResult[i+1] = (l_True == minisatSolver.model[i]) ? (i+1):-(i+1);
        }

        set<int> unsatSoftClauses;
        unsigned long long actualWeightRelaxed = findUnsatClauseCount(finalFormattedResult, dimParser->getOriginalClauses(),
                unsatSoftClauses, verbose_level);

        if (verbose_level > 0){
            cout << "c number of softclauses unsat = " << unsatSoftClauses.size() << std::endl;
            cout << "o " << actualWeightRelaxed << std::endl;
        }

        if (verbose_level > 2){
            cout << "v ";
            for (int i = 1; i <= dimParser->getOriginalVarCount(); i++) {
                assert(finalFormattedResult[i] != 0) ;
                cout << finalFormattedResult[i] << " ";
            }
            cout << std::endl << std::flush;
        }

    }catch (exception& e) {
        cerr << "get_initial_search_space_using_solver:" << e.what() << std::endl << std::flush;
    } catch (...) {
        cerr << "get_initial_search_space_using_solver: Exception occurred."<< std::endl << std::flush;
    }
}
*/
#endif
