#include "cmd_flags.h"
#include "io.h"
#include "assert.h"
#include "string.h"
#include "stdlib.h"
#include "stdio.h"

const char* const _help = R"(
Usage: qdr 
		--help Display options
		--accuracy *accuracy* Set output accuracy (console only)
		--test Run unit tests
		--seed *seed* Set random tests seed
		--ai Run AI mode (funny) console only
		--no-gui Console mode
)";

int ProcessFlags(int argc, char* argv[],
	 int* accuracy, bool* need_test, bool* is_ai, 
	 int* seed, bool* is_nogui){
	assert(accuracy != NULL);
	assert(argv != NULL);
	assert(need_test != NULL);
	assert(is_ai != NULL);
	assert(seed != NULL);
	assert(is_nogui != NULL);

	for (int i = 1; i < argc; i++){
		if (strcmp(argv[i], "--accuracy") == 0 && i+1 < argc && CheckStringOfDigits(argv[i+1])){
			*accuracy = atoi(argv[i+1]);
			i+=1;
		}
		else if (strcmp(argv[i], "--test") == 0){ 
			*need_test = true;
		}else if (strcmp(argv[i], "--seed") == 0 && i+1 < argc && CheckStringOfDigits(argv[i+1])){
			*seed = atoi(argv[i+1]);
			i+=1;
		}else if(strcmp(argv[i], "--ai") == 0){
			*is_ai = true;
		}
		else if(strcmp(argv[i], "--no-gui") == 0){
			*is_nogui = true;
		}
		else if(strcmp(argv[i], "--help") == 0){
			printf(_help);
			return -1;
		}
		else{
			ColoredPrintf(WHITE, RED, "Invalid options\nUse qdr --help\n");
			return -1;
		}
	}
	return 0;
}