#include "basis_pms.h"
#include "pms.h"
#include <signal.h>
#include <unistd.h>

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

	signal(SIGTERM,interrupt);

	int verbose_level = 0;
	int max_time_to_run = 300;
	bool originalCode = false;
	bool randomWithEveryRun=false;
	int t = -1;
	float sp = -1.0;
	int hinc = -1;
	int eta = -1;
	int max_search=-1;
	bool verification_to_be_done = false;
	bool print_final_var_assignment = false;

	// Shut GetOpt error messages down (return '?'):
	opterr = 0;
	int opt;
	// Retrieve the options:
	cout << "SELECTED-OPTIONS: ";
	while ( (opt = getopt(argc, argv, "v:pcm:ort:s:h:e:z:")) != -1 ) {  // for each option...
	    switch ( opt ) {
	    case 'v':
	        verbose_level = atoi(optarg);
            cout << " verbose_level=" << verbose_level;
            break;
	    case 'p':
	        print_final_var_assignment = true;
	        cout << " print_final_var_assignment";
	        break;
	    case 'c':
	        verification_to_be_done = true;
	        cout << " auto-verification-of-solution";
	        break;
	    case 'm':
	        max_time_to_run = atoi(optarg);
	        cout << " max_time_to_run=" << max_time_to_run;
	        break;
	    case 'o':
	        originalCode = true;
	        cout << " original-code";
	        break;
	    case 'r':
	        randomWithEveryRun = true;
	        cout << " random-every-run";
	        break;
	    case 't':
	        t = atoi(optarg);
	        cout << " t=" << t;
	        break;
	    case 's':
	        sp = atof(optarg);
	        cout << " sp=" << sp;
	        break;
	    case 'h':
	        hinc = atoi(optarg);
	        cout << " hinc=" << hinc;
	        break;
	    case 'e':
	        eta = atoi(optarg);
	        cout << " eta=" << eta;
	        break;
	    case 'z':
	        max_search = atoi(optarg);
	        cout << " max_search=" << max_search;
	        break;
	    case '?':  // unknown option...
	        cerr << "Unknown option: '" << char(optopt) << "'!" << endl;
	        break;
	    default:break;//do nothing
	    }
	}
	cout<<endl<<flush;

	assert(optind == (argc-1));
	s.build_instance(argv[optind]);

	if(originalCode){
	    vector<int> init_solution;
	    s.local_search_with_decimation(init_solution,argv[optind], randomWithEveryRun, max_time_to_run, verbose_level, verification_to_be_done); //ORIGINAL
	}else{
	    s.local_search_with_decimation_using_steps(randomWithEveryRun, max_time_to_run,
	            t, sp, hinc, eta, max_search, verbose_level, verification_to_be_done);
	}
	s.print_best_solution(print_final_var_assignment);
	s.free_memory();
	
    return (0);
}
