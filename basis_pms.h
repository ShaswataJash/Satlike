#ifndef _BASIS_PMS_H_
#define _BASIS_PMS_H_


#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <cmath>
#include <cstring>
#include <queue>
#include <stdio.h>
#include <stdlib.h>
#include <sys/times.h> //these two h files are for timing in linux
#include <unistd.h>
#include <cassert>
#include <random>

using namespace std;

class Decimation; //Shaswata - forward declaration

/*
#include "DimacsParser.h" //Shaswata
#include "core/Solver.h" //for minisat
using namespace Minisat;
*/

#define mypop(stack) stack[--stack ## _fill_pointer]
#define mypush(item, stack) stack[stack ## _fill_pointer++] = item

//const float       MY_RAND_MAX_FLOAT = 10000000.0; //Shaswata - not used anywhere
const int   	  MY_RAND_MAX_INT =   10000000;
const float 	  BASIC_SCALE = 0.0000001; //1.0f/MY_RAND_MAX_FLOAT;

// Define a data structure for a literal.
struct lit 
{
    int             clause_num;		//clause num, begin with 0
    int             var_num;		//variable num, begin with 1
    bool             sense;			//is 1 for true literals, 0 for false literals.
};

class Satlike
{
	private:

	/***********non-algorithmic information ****************/
	int problem_weighted;
	int partial; //1 if the instance has hard clauses, and 0 otherwise.

	int max_clause_length;
	int min_clause_length;

	//size of the instance
	int     num_vars;		//var index from 1 to num_vars
	int     num_clauses;		//clause index from 0 to num_clauses-1
	int     num_hclauses;
	int     num_sclauses;

	//steps and time
	struct tms start_time; //Shaswata - avoiding global variables for accessing from python
	int max_tries;
	unsigned int INITIAL_MAX_FLIP;
	unsigned int max_flips;
	unsigned int max_non_improve_flip;
	//unsigned int step; //Shaswata - no need to present in class variable, can be defined locally in function

	int print_time;
	int print_time1;
	int print_time2;
	int print1;
	int print2;
	int cutoff_time;
	int cutoff_time1;
	int cutoff_time2;
	int prioup_time;
	double  opt_time;

	/**********end non-algorithmic information*****************/
	/* literal arrays */				
	lit**	var_lit;				//var_lit[i][j] means the j'th literal of var i.
	int*	var_lit_count;			//amount of literals of each var
	lit**	clause_lit;			//clause_lit[i][j] means the j'th literal of clause i.
	int*	clause_lit_count; 			// amount of literals in each clause


	/* Information about the variables. */
	long long*	score;		
	long long*	time_stamp;
	int**	var_neighbor;
	int*	var_neighbor_count;
	int*	neighbor_flag;
	int*	temp_neighbor;

	/* Information about the clauses */		
	long long	top_clause_weight;
	long long*	org_clause_weight;	
	long long	total_soft_weight;
	long long*	clause_weight;		
	int*	sat_count;			
	int*	sat_var;		
	long long*	clause_selected_count;
	int*	best_soft_clause;

	//original unit clause stack
	lit* unit_clause;
	int unit_clause_count;

	//unsat clauses stack
	int*	hardunsat_stack;		//store the unsat clause number
	int*	index_in_hardunsat_stack;//which position is a clause in the unsat_stack
	int 	hardunsat_stack_fill_pointer;

	int*	softunsat_stack;		//store the unsat clause number
	int*	index_in_softunsat_stack;//which position is a clause in the unsat_stack
	int 	softunsat_stack_fill_pointer;

	//variables in unsat clauses
	int*	unsatvar_stack;		
	int		unsatvar_stack_fill_pointer;
	int*	index_in_unsatvar_stack;
	int*	unsat_app_count;		//a varible appears in how many unsat clauses

	//good decreasing variables (dscore>0 and confchange=1)
	int*	goodvar_stack;		
	int		goodvar_stack_fill_pointer;
	int*	already_in_goodvar_stack;

	/* Information about solution */	
	int*    cur_soln;	//the current solution, with 1's for True variables, and 0's for False variables
	int*    best_soln;
	int*	local_opt_soln;
	int     best_soln_feasible; //when find a feasible solution, this is marked as 1.
	int		local_soln_feasible;
	int     hard_unsat_nb;
	long long    soft_unsat_weight;
	long long    opt_unsat_weight;
	long long	 local_opt_unsat_weight;

	//clause weighting 
	int*   large_weight_clauses;
	int    large_weight_clauses_count;	
	int    large_clause_count_threshold;

	int*   soft_large_weight_clauses;
	int*   already_in_soft_large_weight_stack;
	int    soft_large_weight_clauses_count;	
	int    soft_large_clause_count_threshold;

	//tem data structure used in algorithm
	int* best_array;
	int  best_count;
	int* temp_lit;

	//parameters used in algorithm
	float rwprob;
	float rdprob;
	float  smooth_probability;
	int hd_count_threshold;
	int h_inc;
	int softclause_weight_threshold;
	int feasible_flag;

	//Decimation
	Decimation* deci; //Shaswata
	vector<int> init_solution; //Shaswata

	//Shaswata - not to depend upon any global state within standard lib (becomes problematic from python)
	std::mt19937* generator;

	//function used in algorithm
	void build_neighbor_relation();
	void allocate_memory();
	bool verify_sol();
	void increase_weights();
	void smooth_weights();
	void update_clause_weights();
	void unsat(int clause);
	void sat(int clause);
	void init(vector<int>& init_solution, bool override_init_sol=false);//Shaswata: added additional override_init_sol
	void flip(int flipvar);
	void update_goodvarstack1(int flipvar);
	void update_goodvarstack2(int flipvar);
	int pick_var();
	void settings(bool debug=false);
	void update_hyper_param(int t, float sp, int hinc, int eta, int max_search);//Shaswata - for RL

	double get_runtime()
	{
	    struct tms stop;
	    times(&stop);
	    return (double)(stop.tms_utime-start_time.tms_utime+stop.tms_stime-start_time.tms_stime)/sysconf(_SC_CLK_TCK);
	}

	unsigned int my_get_rand(){
	    unsigned int r = (*generator)();
	    return (r);
	}

	int prev_hard_unsat_nb;
	long long prev_soft_unsat_weight;
	int fliped_var;


	//=================== Shaswata ================
	/*
	int* finalFormattedResult ;
	WeightedDimacsParser* dimParser;
	void addOnlyHardClausesToSolver(Solver& solver);
	void get_initial_search_space_using_solver(const char* inputfile, int verbose_level=0);*/
	//=============================================

	public:

	void build_instance(const char *filename); //interface for python
	void local_search(vector<int>& init_solution);
	void local_search_with_decimation(unsigned int seed, vector<int>& init_solution, const char* inputfile,
	        int max_time_to_run=300,
	        int verbose = 0, bool verification_to_be_done = false);

	void algo_init(unsigned int seed, bool todebug);//Shaswata - interface for python
	void init_with_decimation_stepwise(bool through_decimation=true);//Shaswata	- interface for python
	long long local_search_stepwise(int t, float sp,  int hinc, int eta, int max_search,
	        unsigned int current_step, bool adaptive_search_extent=true, int verbose=0);//Shaswata - interface for python

	//Following function is to compare behavior of our stepwise modification with local_search_with_decimation
	void local_search_with_decimation_using_steps(unsigned int seed, int maxTimeToRunInSec=300,
	        int t=-1, float sp=-1,  int hinc=-1, int eta=-1, int max_search = -1,
	        bool adaptive_search_extent=true, int verbose_level=0, bool verification_to_be_done = false);//Shaswata

	void simple_print();
	void print_best_solution(bool print_var_assign = false);
	void free_memory(); //interface for python

	//Shaswata - interface for python
	void set_initial_max_flip(unsigned int v) { INITIAL_MAX_FLIP = v;}
	unsigned int get_max_flips() { return (max_flips);}
	unsigned int get_max_non_improve_flip() { return (max_non_improve_flip);}
	float get_smooth_probability() { return (smooth_probability);}
	int get_hd_count_threshold() { return (hd_count_threshold);}
	int get_h_inc() { return (h_inc);}
	int get_softclause_weight_threshold() { return (softclause_weight_threshold);}

	int  get_num_vars(){ return (num_vars);}
	int  get_num_clauses() {return (num_clauses);}
	int  get_num_hclauses() {return (num_hclauses);}
	int  get_num_sclauses() {return (num_sclauses);}

	int  get_hard_unsat_nb() { return (hard_unsat_nb);}

	long long get_top_clause_weight() { return (top_clause_weight);}
	long long get_total_soft_weight() { return (total_soft_weight);}
	long long get_soft_unsat_weight() { return (soft_unsat_weight);}
	long long get_opt_unsat_weight() { return (opt_unsat_weight);}

	int get_hard_unsat_nb_before_flip() {return (prev_hard_unsat_nb);}
	long long get_soft_unsat_weight_before_flip() {return (prev_soft_unsat_weight);}
	int get_flipped_var(){return (fliped_var);}

	long long get_var_score(int v){return (score[v]);}
	long long get_clause_weight(int c){return (clause_weight[c]);}

	int get_current_sol(int var_id) { return (cur_soln[var_id]);}
	int get_best_sol(int var_id){ return (best_soln[var_id]);}
	int get_init_sol(int var_id){ return (init_solution[var_id]);}
	void set_init_sol(int var_id, int assignment){init_solution[var_id] = assignment;}

	int get_feasible_flag_state() { return (feasible_flag);}

	Satlike();
	~Satlike(){free_memory();}

};


#endif

