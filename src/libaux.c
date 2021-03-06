/*

					Implementação da Biblioteca auxiliar da GaVini <3   v1.0.0.0
					Tem que revisar :)
	
*/
#include <ucontext.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <stdarg.h>
#include "libaux.h"
//////////////////////////////////////////////////////////////////////////
//
//			Retorna a thread atual a partir do ID global
//
//////////////////////////////////////////////////////////////////////////
THREAD_t* current_thread() {
	if(!setIterator(lstApto, threadAtualID))
		return (THREAD_t*) GetAtIteratorFila2(lstApto);
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
	//a coloca na lista de bloqueados.
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
		AppendFila2(lstApto, (void*) thread);
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
	*(current_thread()->context) = *main_context;

	/* retorna para main */
	return (void) setcontext(current_thread()->context);
}
//////////////////////////////////////////////////////////////////////////
//
//			Aloca uma nova thread ?????
//
//////////////////////////////////////////////////////////////////////////
void allocate_thread_make()
{
	/* aloca variavel e contexto. */
    allocator_buffer = (THREAD_t*) malloc(sizeof(THREAD_t));
    allocator_buffer->context = (ucontext_t*) malloc(sizeof(ucontext_t));

	/* salva contexto */
    getcontext(allocator_buffer->context);

	/* Aloca pilha */
    allocator_buffer->stack = (char*) malloc(CTHREADS_STACK_SIZE);

	allocator_buffer->context->uc_stack.ss_sp = allocator_buffer->stack;
	allocator_buffer->context->uc_stack.ss_size = CTHREADS_STACK_SIZE;

	/* quando terminar vai para terminate_context */
	allocator_buffer->context->uc_link = terminateContext;
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

