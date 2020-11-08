#include "basis_pms.h"
#include "pms.h"
#include <signal.h>

static Satlike s;

void interrupt(int sig)
{
	s.print_best_solution();
	s.free_memory();
	exit(10);
}

int main(int argc, char* argv[])
{
	cout<<"This is Satlike3.0 solver"<<endl<<flush;
	vector<int> init_solution;
	signal(SIGTERM,interrupt);
	s.build_instance(argv[1]);
	//s.local_search_with_decimation(init_solution,argv[1]); //ORIGINAL
	int max_time_to_run = (argc == 3)? atoi(argv[2]) : 60;
	s.local_search_with_decimation_using_steps(true, false, max_time_to_run);
	s.print_best_solution();
	s.free_memory();
	
    return 0;
}
