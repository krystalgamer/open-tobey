#include "my_assertions.h"
#include <cstdio>

int FAIL_VALIDATION = 0;

void validate_i32(int cur, int expected, const char *name, int line)
{
	if (expected != cur)
	{
		FAIL_VALIDATION = 1;
		printf("[!] Expected %d but got %d: %s:%d\n", expected, cur, name, line);
	}
}

void validate_class(int cur, int expected, const char *cls, const char *member){


	if (expected != cur){

		FAIL_VALIDATION = 1;

		printf("[!] Expected %X but got %X, for %s->%s\n", expected, cur, cls, member);
	}
	else{

#ifdef _VALIDATE_SUCCESS
		printf("Success: Expected %X  for %s->%s\n", expected, cls, member);
#endif
	}

	fflush(stdout);
}

void validate_size(int cur, int expected, const char *name)
{
	if (expected != cur){
		FAIL_VALIDATION = 1;
		printf("[!] Expected %d but got %d, size of %s\n", expected, cur, name);
	}
	else{

#ifdef _VALIDATE_SUCCESS
		printf("[!] SUGG my dicgg\n");
#endif
	}

	fflush(stdout);
}


