#include "cdata.h"
#include "fila2.h"
#include "cthread.h"
#define IDNULL 0
#define TRUE 1
#define FALSE 0

//para ver se a thread main já foi criada :)
int mainInit;

PFILA lstApto, lstBlock;

/*NÃO ESTÁ NEM PERTO DE ESTAR FINALIZADA, É SO PRA IR ENTENDENDO*/

ucontext_t *terminateContext, *allocatorContext;

int threadAtualID;


//NÃO ESTÁ PRONTA-------------

int ccreate (void* (*start)(void*), void *arg) {
	if(!mainInit){
		mainInit = TRUE;

		terminateContext = allocate_context();

	}
}


//PRONTA

int csem_init(csem_t *sem, int count) {
	//ver se main existe, senao retorna erro
	if (!mainInit) {
		sem->count = count;
		//fazer malloc decentemente
		CreateFila2(sem->fila);
		return SUCCESS;
	}
	else
		return ERROR;
}

//PRONTA

int cyield(void){
	if(!mainInit)
		return ERROR;
	int retorno = SUCCESS;
	//Se o tamanho da lista de aptos é maior que um, reescalona, se tiver só uma thread
	//ele continua a execução, pois ela seria a próxima a ser executada
	if(sizeFila(lstApto) > 1)
		retorno = escalonar();

	return retorno;
}


//PRONTA
int cwait(csem_t *sem) {
	int retorno;
	//Se não há semáforo ou a main não existe, retorna erro
	if (!initialized OR sem = NULL)
		return ERROR;
	//Se não há recurso disponível, sem->count < 1...
	if(sem->count < 1) {
		//Coloca a thread atual na lista de espera do semáforo
		AppendFila2(sem->fila, current_thread());
		cBlock();
		retorno = reescalona();
	}
	sem->count--;
	current_thread()->semaforoUsado = sem;
	return retorno;
}


//PRONTA

int cjoin(int tid)
{
    if(!mainInit)
        return ERROR;
    //A variável abaixo indica se a thread a ser esperada já é esperada por outra thread
    int thread_join_free = FALSE;
    THREAD_t* p;

	if (tid != IDNULL && tid < global_thread_id) {
		int thread_was_found = FALSE;
		//Procura a thread a ser esperada na lista de Aptos
		if(!setIterator(lstApto, tid)){
			p = GetAtIterator2(lstApto);
			//Se acha, vê se há uma thread já esperando ela
			if(p->esperadoPor == IDNULL)
    			thread_join_free = TRUE;
    		//Seta o esperadoPor da thread com o id da thread atual
    		p->esperadoPor = current_thread()->threadCB->id;
			thread_was_found = TRUE;
		}
		//Mesmo procedimento para a lista de blocked	
		if (!setIterator(lstBlock, tid)){
    		p = GetAtIterator2(lstBlock);
    			if(p->esperadoPor == IDNULL)
        			thread_join_free = TRUE;
        		p->esperadoPor = current_thread()->threadCB->id;
				thread_was_found = TRUE;
		}
		// Thread a ser esperada já terminou ou non ecsiste
		if (!thread_was_found)
			return ERROR;

		if (thread_join_free){
		/* Thread encontrada e não está sendo esperada por outra, seta o idEsperado e move para fila de block*/
			current_thread()->idEsperado = tid;
			block_thread();
			return escalonar();
		}
	}
	//Caso id seja inválido ou haja outra thread esperando esta, retorna erro
	return ERROR;
}

//PRONTA
int csignal(csem_t *sem){
	if(!mainInit or sem = NULL)
		return ERROR;
	//Se há threads esperando por recurso
	if(sem->fila->first != NULL){
		//Pega e deleta da lista de bloqueados do semáforo a primeira thread
		THREAD_t* aux = sem->fila->first;
		FirstFila2(sem->fila);
		DeleteAtIteratorFila2(sem->fila);
		//E dá unblock nele, colocando-a na fila de aptos
		setIterator(lstBlock, aux->threadCB->id);
		aux = GetAtIterator2(lstBlock);
		cUnblock(aux);
	}
	//incrementa cont, liberando recurso, e seta recurso sendo usado pela thread atual para NULL
	sem->count++;
	current_thread()->semaforoUsado = NULL;
	return SUCCESS;
	
}



THREAD_t* current_thread() {
	THREAD_t* p;
	if(!setIterator(lstApto, threadAtualID))
		return (THREAD_t*) GetAtIterator2(lstApto);
	return NULL;
}	

void cBlock() {
	THREAD_t *aux = current_thread();
	if(aux != NULL)
		aux->threadCB->state = PROCST_BLOQ;
}



int cUnblock(THREAD_t *thread){
	if(thread == NULL)
		return CTHREAD_ERROR_CODE;
	if(!setIterator(lstBlock, thread->threadCB->id)){
		DeleteAtIteratorFila2(lstBlock);
		AppendFila2(lstApto, (void*) thread);
		thread->threadCB->state = CTHREAD_READY;
		return SUCCESS;
	}
	return ERROR;
}


int setIterator(PFILA2 fila, int id){
	PNODE2 aux;

	for(aux = fila->first; aux != NULL; aux = aux->next){
		if(aux->node->threadCB->id == id) {
			fila->it = aux;
			return SUCCESS;
		}
	}
	return ERROR;
}

ucontext_t* allocate_context()
{
	ucontext_t* context = (ucontext_t*) malloc (sizeof(ucontext_t));

	if (getcontext(context) != 0 || context == NULL)
		return NULL;

	/* aloca pilha */
	context->uc_stack.ss_sp = (char*) malloc(MTHREADS_STACK_SIZE);
	context->uc_stack.ss_size = MTHREADS_STACK_SIZE;

	context->uc_link = NULL;

	return context;
}

void free_context(ucontext_t* context)
{
	if (context != NULL)
	{
		free(context->uc_stack.ss_sp);
		free(context);
	}
}


int sizeFila(PFILA2 fila){
	PNODE2 aux;
	int retorno = 0;
	for(aux = fila->first; aux != NULL; aux = aux->next)
		retorno++;
	return retorno;
}

void terminate_current_thread()
{
	THREAD_t* thread = current_thread();
	thread->threadCB->state = PROCST_TERMINO;

    /* se ainda existe um semáforo nessa thread */
    if(thread->semaforoUsado != NULL)
    {
        csignal(thread->semaforoUsado); /* libera o recurso utilizado */
    }

	int idJoined = thread->esperadoPor;
	THREAD_t *aux = NULL;

	if (idJoined != IDNULL)
	{
		if(!setIterator(lstApto, idJoined)) 
			aux = GetAtIterator2(lstApto);
		
		if(!setIterator(lstBlock, idJoined))
			aux = GetAtIterator2(lstBlock);

		aux->idEsperado = IDNULL;
		/* Desbloqueia thread que estava esperando pelo fim da thread em execução. */
		unblock_thread(aux);
	}
	/* Chama escalonador, e executa proxima thread. */
	reschedule();
}

//NAO SEI SE E
int terminateContext() {
	THREAD_t *thread = threadAtual();
	thread->thread->state = PROCST_TERMINO;

	if(thread->isUsingRecurso) {
		csignal(sem);
	}

}











}