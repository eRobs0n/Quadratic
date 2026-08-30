#include "cmd_flags.h"
#include "io.h"
#include "assert.h"
#include "string.h"
#include "stdlib.h"

int ProcessFlags(int argc, char* argv[], int* accuracy, bool* need_test, bool* is_ai, int* seed){
	assert(accuracy != NULL);
	assert(argv != NULL);
	assert(need_test != NULL);
	assert(is_ai != NULL);
	assert(seed != NULL);

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
		else{
			//printf("%s\n", argv[i]);
			ColoredPrintf(WHITE, RED, "Usage: qdr --accuracy *accuracy* --test\n");
			return -1;
		}
	}
	return 0;
}