#define IDNULL 0
#define TRUE 1
#define FALSE 0

/*

					Interface da Biblioteca auxiliar da GaVini <3   v1.0.0.0
					Tem que revisar :)
					Falta a função escalonar();!
	
*/
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <stdarg.h>
#include "cthread.h"
#include "cdata.h"

//Variáveis usadas em TODO O LUGAR, PQ VCS VALEM MT
//para ver se a thread main já foi criada :)
int mainInit;
//Filas de apto e block gerais
PFILA2 lstAptos, lstBlock;
//Contexto que todas as threads vão quando terminam
ucontext_t* terminateContext;
//Contexto no qual todas as threads são alocadas e desalocadas 
ucontext_t allocatorContext;
/* Variavél auxiliar para a função allocator_context. IDEIA DO VINI, NAO SEI SE EH NECESARIO*/
THREAD_t* allocator_buffer;
// O id que a nova thread criada receberá.
int globalIDCount = 0;
int threadAtualID = 0;


//Pega a thread atual na lista de aptos 
THREAD_t* current_thread();
//Bloqueia a thread atual
void cBlock();
//Dá unblock na thread passada
int cUnblock(THREAD_t *thread);
//Seta o iterador da fila na thread com o id especificado
int setIterator(PFILA2 fila, int id);
//Dada uma fila, retorna o tamanho da mesma
int sizeFila(PFILA2 fila);
//Termina a thread atual
void terminate_current_thread();
//Seta o novo id da thread
int get_new_id();
//Executa próxima thread
int escalona();
void free_thread_private();
void free_thread(THREAD_t* thread);

//A partir daqui não estão revisadas, talvez seja necessário dar uma olhada...

//Cria a thread main
void create_main_thread(void* context);

//Aloca uma thread
THREAD_t* allocate_thread();

//Dá free em um contexto
void free_context(ucontext_t* context);


//Aloca um novo contexto
ucontext_t* allocate_context();





/*

						Biblioteca de Threads da Gabi Motoquera e do André   v1.0.0.0
						Inclui csem_init, cyield, cjoin, cwait, csignal.
						Falta: ccreate (para adquirir esse pacote por favor ligue para 0800 1)
	
*/



//NÃO ESTÁ PRONTA-------------

int ccreate (void* (*start)(void*), void *arg){
	return ERROR;
}


//PRONTA

int csem_init(csem_t *sem, int count) {
	//ver se main existe, senao retorna erro
	if (mainInit) {
		sem->count = count;
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
	if(sizeFila(lstAptos) > 1)
		retorno = escalona();

	return retorno;
}


//Função a ser chamada
int cwait(csem_t *sem) {
	int retorno;
	//Se não há semáforo ou a main não existe, retorna erro
	if (!mainInit || sem == NULL)
		return ERROR;
	//Se não há recurso disponível, sem->count < 1...
	if(sem->count < 1) {
		//Coloca a thread atual na lista de espera do semáforo
		AppendFila2(sem->fila, current_thread());
		cBlock();
		retorno = escalona();
	}
	sem->count--;
	current_thread()->semaforoUsado = sem;
	return retorno;
}


//PRONTA

int cjoin(int tid)
{
	//Se a main não está iniciada e o id é inválido, retorna ERROR
    if(!mainInit || tid == IDNULL)
        return ERROR;
    //A variável abaixo indica se a thread a ser esperada já é esperada por outra thread
    int thread_join_free = FALSE;
    //A variável vai ser setada para true se existe uma thread com o ID pedido
	int thread_was_found = FALSE;
	THREAD_t* aux;
	//Procura a thread a ser esperada na lista de Aptos
	if(!setIterator(lstAptos, tid)){
		aux = (THREAD_t *) GetAtIteratorFila2(lstAptos);
		//Se acha, vê se há uma thread já esperando ela
		if(aux->waitedJoin != NULL)
			thread_join_free = TRUE;
		//Seta o esperadoPor da thread com o id da thread atual
		aux->waitedJoin = current_thread();
		thread_was_found = TRUE;
	}
	//Mesmo procedimento para a lista de blocked	
	if (!setIterator(lstBlock, tid)){
		aux = (THREAD_t *) GetAtIteratorFila2(lstBlock);
			if(aux->waitedJoin != NULL)
    			thread_join_free = TRUE;
    		aux->waitedJoin = current_thread();
			thread_was_found = TRUE;
	}
	// Thread a ser esperada já terminou ou non ecsiste
	if (!thread_was_found)
		return ERROR;

	if (thread_join_free){
	/* Thread encontrada e não está sendo esperada por outra, seta o idEsperado e move para fila de block*/
		cBlock();
		return escalona();
	}
	//Caso id seja inválido ou haja outra thread esperando esta, retorna erro
	return ERROR;
}


int csignal(csem_t *sem){
	//Se a main não está iniciada ou o semáforo é NULL
	if(!mainInit || sem == NULL)
		return ERROR;
	//Se há threads esperando por recurso
	if(sem->fila->first != NULL){
		//Pega e deleta da lista de bloqueados do semáforo a primeira thread
		THREAD_t* aux = (THREAD_t*) sem->fila->first;
		FirstFila2(sem->fila);
		DeleteAtIteratorFila2(sem->fila);
		//E dá unblock nele, colocando-a na fila de aptos
		setIterator(lstBlock, aux->threadCB->tid);
		aux = GetAtIteratorFila2(lstBlock);
		cUnblock(aux);
	}
	//incrementa cont, liberando recurso, e seta recurso sendo usado pela thread atual para NULL
	sem->count++;
	current_thread()->semaforoUsado = NULL;
	return SUCCESS;
	
}



/*

					Implementação da Biblioteca auxiliar da GaDré <3   v1.0.0.0
					Tem que revisar :)
	
*/
//////////////////////////////////////////////////////////////////////////
//
//			Retorna a thread atual a partir do ID global
//
//////////////////////////////////////////////////////////////////////////
THREAD_t* current_thread() {
	if(!setIterator(lstAptos, threadAtualID))
		return (THREAD_t*) GetAtIteratorFila2(lstAptos);
	return NULL;
}	

//////////////////////////////////////////////////////////////////////////
//
//			Dá block na thread atual (talvez seja necessário fazer o tratamento da lstBlock)
//
//////////////////////////////////////////////////////////////////////////
void cBlock() {
	//Pega a thread atual e seta seu estado para bloqueado.
	THREAD_t *aux = current_thread();
	if(aux != NULL)
		aux->threadCB->state = PROCST_BLOQ;
	//a coloca na lista de bloqueados do semáforo ou global, depende da vontade do freguês
	AppendFila2(lstBlock, (void *)aux);
}

//////////////////////////////////////////////////////////////////////////
//
//		Dá unblock em uma thread passada
//
//////////////////////////////////////////////////////////////////////////
int cUnblock(THREAD_t *thread){
	//Se a thrread nao existe ela retorna ERROR
	if(thread == NULL)
		return ERROR;
	//Seta o iterador da lista de bloqueados na id, se achar
	// e coloca a thread na lista de aptos
	//por fim, troca o estado de bloqueado para READY
	if(!setIterator(lstBlock, thread->threadCB->tid)){
		DeleteAtIteratorFila2(lstBlock);
		AppendFila2(lstAptos, (void*) thread);
		thread->threadCB->state = PROCST_APTO;
		return SUCCESS;
	}
	return ERROR;
}
//////////////////////////////////////////////////////////////////////////
//
//		Seta o iterador da fila em um dado ID
//
//////////////////////////////////////////////////////////////////////////

int setIterator(PFILA2 fila, int id){
	PNODE2 aux;

	for(aux = fila->first; aux != NULL; aux = aux->next){
		if(((THREAD_t *)aux->node)->threadCB->tid == id) {
			fila->it = aux;
			return SUCCESS;
		}
	}
	return ERROR;
}



//////////////////////////////////////////////////////////////////////////
//
//		Retorna o tamanho de uma fila
//
//////////////////////////////////////////////////////////////////////////
int sizeFila(PFILA2 fila){

	PNODE2 aux;
	int retorno = 0;
	for(aux = fila->first; aux != NULL; aux = aux->next)
		retorno++;
	return retorno;
}


//////////////////////////////////////////////////////////////////////////
//
//		Termina a thread atual
//
//////////////////////////////////////////////////////////////////////////
void terminate_current_thread(){
	//Pega a thread atual e seta seu estado para TERMINANDO
	THREAD_t* thread = current_thread();
	thread->threadCB->state = PROCST_TERMINO;

    // se ainda existe um semáforo nessa thread, ela libera o recurso
    // e seta o semaforo para NULL
    if(thread->semaforoUsado != NULL)
        csignal(thread->semaforoUsado); 
    thread->semaforoUsado = NULL;

	THREAD_t* joined = thread->waitedJoin;
	//Se há uma thread esperando por essa
	if (joined != NULL)
		cUnblock(joined);
	// Chama escalonador, e executa proxima thread. 
	escalona();
}
//////////////////////////////////////////////////////////////////////////
//
//			Seta o novo id da thread
//
//////////////////////////////////////////////////////////////////////////
int get_new_id()
{
	int id = globalIDCount;
	++globalIDCount;
	return id;
}
//////////////////////////////////////////////////////////////////////////
//
//			Cria a main thread ????
//
//////////////////////////////////////////////////////////////////////////
void create_main_thread(void* context)
{
	ucontext_t* main_context = (ucontext_t*) context;
	current_thread()->threadCB->context = *main_context;

	/* retorna para main */
	return (void) setcontext(&current_thread()->threadCB->context);
}
//////////////////////////////////////////////////////////////////////////
//
//			Aloca uma nova thread 
//
//////////////////////////////////////////////////////////////////////////
void allocate_thread_make()
{
	ucontext_t* aux;
	/* aloca variavel e contexto. */
    allocator_buffer = (THREAD_t*) malloc(sizeof(THREAD_t));
    aux = (ucontext_t*) malloc(sizeof(ucontext_t));
	allocator_buffer->threadCB->context = *(aux);
	/* salva contexto */
    getcontext(&allocator_buffer->threadCB->context);

	/* Aloca pilha */
    allocator_buffer->stack = (char*) malloc(CTHREADS_STACK_SIZE);

	allocator_buffer->threadCB->context.uc_stack.ss_sp = allocator_buffer->stack;
	allocator_buffer->threadCB->context.uc_stack.ss_size = CTHREADS_STACK_SIZE;

	/* quando terminar vai para terminate_context */
	allocator_buffer->threadCB->context.uc_link = terminateContext;
}

THREAD_t* allocate_thread()
{
    ucontext_t current_context;

    int jaAlocada = FALSE;

	/* salva contexto. */
    getcontext(&current_context);

	/* Se já foi alocado não entra no if */
    if (!jaAlocada)
    {
		jaAlocada = TRUE;
        allocatorContext.uc_link = &current_context;
		/* seta que allocator_context começa na função allocate_thread_private */
        makecontext(&allocatorContext,
			(void(*)(void))allocate_thread_make, 0);

        setcontext(&allocatorContext);
    }

    return allocator_buffer;
}
//////////////////////////////////////////////////////////////////////////
//
//		Dá free em um contexto
//
//////////////////////////////////////////////////////////////////////////
void free_context(ucontext_t* context){
	if (context != NULL){
		free(context->uc_stack.ss_sp);
		free(context);
	}
}


//////////////////////////////////////////////////////////////////////////
//
//		Alloca um novo contexto
//
//////////////////////////////////////////////////////////////////////////
ucontext_t* allocate_context()
{
	ucontext_t* context = (ucontext_t*) malloc (sizeof(ucontext_t));

	if (getcontext(context) != 0 || context == NULL)
		return NULL;

	//aloca a pilha do contexto, NAO SEI COMOFAS ANDRÉ PLS
	context->uc_stack.ss_sp = (char*) malloc(CTHREADS_STACK_SIZE);
	context->uc_stack.ss_size = CTHREADS_STACK_SIZE;

	context->uc_link = NULL;

	return context;
}

//////////////////////////////////////////////////////////////////////////
//
//		Pega a próxima thread e a coloca para execução
//
//////////////////////////////////////////////////////////////////////////
int escalona()
{
	FirstFila2(lstAptos);
	THREAD_t* thread = (THREAD_t*) GetAtIteratorFila2(lstAptos);
	DeleteAtIteratorFila2(lstAptos);
	if (thread == NULL)
	{
		printf("ERRO NULL\n");
		exit(ERROR);
	}

	int already_swapped_context = FALSE;
	int code = SUCCESS;

	if (thread->threadCB->state == PROCST_EXEC ||
		thread->threadCB->state == PROCST_CRIACAO)
	{
	    /* yield ou iniciando*/
		thread->threadCB->state = PROCST_APTO;
		AppendFila2(lstAptos, (void *) thread);  

		/* salva o contexto. */
		code |= getcontext(&(thread->threadCB->context));

		/* Retoma aqui */
	}
	else if (thread->threadCB->state == PROCST_TERMINO)
	{
		/* Thread terminando, desaloca aqui. */

		free_thread(thread);
		thread = NULL;
	}
	else if (thread->threadCB->state == PROCST_BLOQ)
	{
		/* Recalcula o tempo de execução e coloca na lista de bloqueados */
		AppendFila2(lstBlock, (void *) thread);

		/* salva contexto. */
		code |= getcontext(&(thread->threadCB->context));

		/* Retoma aqui */
	}
	else if (thread->threadCB->state == PROCST_APTO)
		AppendFila2(lstAptos, (void*) thread);

	/* As threads retomam sua execução aqui, already_swapped_context == TRUE. */
	if (!already_swapped_context)
	{
		already_swapped_context = TRUE;

		/* Pega a primeira da lista de aptos*/
		FirstFila2(lstAptos);
		THREAD_t* first_thread = (THREAD_t*) GetAtIteratorFila2(lstAptos); 

		/* Verifica se é valida */
		if (first_thread == NULL || first_thread->threadCB->tid < 0)
		{
			printf("THREADS ARE DEADLOCKED!"
				   " Program will now exit.\n");
			exit(ERROR);
		}

		threadAtualID = first_thread->threadCB->tid;
		first_thread->threadCB->state = PROCST_EXEC;

		/* Troca contexto pra nova thread em execução */
		code |= setcontext(&(first_thread->threadCB->context));
	}

	return code;
}



///FAZER CABEÇALHO


void free_thread_private()
{
    assert(allocator_buffer != NULL);
    free(&(allocator_buffer->threadCB->context));
    free(allocator_buffer->stack);
    free(allocator_buffer);
    allocator_buffer = NULL;
}

/*----------------------------------------------------------------------------*/

void free_thread(THREAD_t* thread)
{
    ucontext_t current_context;
    int already_deallocated = FALSE;
    allocator_buffer = thread;

	/* salva contexto */
    getcontext(&current_context);
    /* Se já foi desalocado não entra no if */
    if (!already_deallocated)
    {
        already_deallocated = TRUE;
        allocatorContext.uc_link = &current_context;

        makecontext(&allocatorContext, (void(*)(void))free_thread_private, 0);
        setcontext(&allocatorContext);
    }
}


