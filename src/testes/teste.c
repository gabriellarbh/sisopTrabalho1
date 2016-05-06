#include <stdlib.h>
#include <stdio.h>
#include "cthread.h"

int counterJob = 0;

void* doSomeThing(void *arg)
{
    printf("\n Job %d started", counterJob);
	
    return NULL;
}

int main(void)
{    
	int id = ccreate(doSomeThing, NULL);
  	cjoin(id);

    printf("\n main finished\n");
    
    return 0;
}
