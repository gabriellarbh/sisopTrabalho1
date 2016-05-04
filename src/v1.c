#define IDNULL 0
#define TRUE 1
#define FALSE 0

/*

					Interface da Biblioteca auxiliar da GaVini <3   v1.0.0.0
					Tem que revisar :)
					Falta a função escalonar();!
	
*/
#include "cdata.h"

//Variáveis usadas em TODO O LUGAR, PQ VCS VALEM MT
//para ver se a thread main já foi criada :)
int mainInit;
//Filas de apto e block gerais
PFILA2 lstApto, lstBlock;
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

int ccreate (void* (*start)(void*), void *arg);


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
	if(sizeFila(lstApto) > 1)
		retorno = escalona();

	return retorno;
}


//Função a ser chamada
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
    if(!mainInit OR tid == IDNULL)
        return ERROR;
    //A variável abaixo indica se a thread a ser esperada já é esperada por outra thread
    int thread_join_free = FALSE;
    //A variável vai ser setada para true se existe uma thread com o ID pedido
	int thread_was_found = FALSE;
	THREAD_t* aux;
	//Procura a thread a ser esperada na lista de Aptos
	if(!setIterator(lstApto, tid)){
		aux = GetAtIterator2(lstApto);
		//Se acha, vê se há uma thread já esperando ela
		if(aux->waitedJoin != NULL)
			thread_join_free = TRUE;
		//Seta o esperadoPor da thread com o id da thread atual
		aux->waitedJoin = current_thread();
		thread_was_found = TRUE;
	}
	//Mesmo procedimento para a lista de blocked	
	if (!setIterator(lstBlock, tid)){
		aux = GetAtIterator2(lstBlock);
			if(aux->waitedJoin != NULL)
    			thread_join_free = TRUE;
    		p->waitedJoin = current_thread();
			thread_was_found = TRUE;
	}
	// Thread a ser esperada já terminou ou non ecsiste
	if (!thread_was_found)
		return ERROR;

	if (thread_join_free){
	/* Thread encontrada e não está sendo esperada por outra, seta o idEsperado e move para fila de block*/
		current_thread()->waitingJoin = GetAtIterator2()
		block_thread();
		return escalonar();
	}
	//Caso id seja inválido ou haja outra thread esperando esta, retorna erro
	return ERROR;
}


int csignal(csem_t *sem){
	//Se a main não está iniciada ou o semáforo é NULL
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



/*

					Implementação da Biblioteca auxiliar da GaDré <3   v1.0.0.0
					Tem que revisar :)
	
*/
#include "libaux.h"
//////////////////////////////////////////////////////////////////////////
//
//			Retorna a thread atual a partir do ID global
//
//////////////////////////////////////////////////////////////////////////
THREAD_t* current_thread() {
	THREAD_t* p;
	if(!setIterator(lstApto, threadAtualID))
		return (THREAD_t*) GetAtIterator2(lstApto);
	return NULL
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
	if(!setIterator(lstBlock, thread->threadCB->id)){
		DeleteAtIteratorFila2(lstBlock);
		AppendFila2(lstApto, (void*) thread);
		thread->threadCB->state = CTHREAD_READY;
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
		if(((THREAD_t *)aux->node)->threadCB->id == id) {
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
		unblock_thread(joined);
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
        allocator_context.uc_link = &current_context;
		/* seta que allocator_context começa na função allocate_thread_private */
        makecontext(&allocator_context,
			(void(*)(void))allocate_thread_private, 0);

        setcontext(&allocator_context);
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
	context->uc_stack.ss_sp = (char*) malloc(MTHREADS_STACK_SIZE);
	context->uc_stack.ss_size = MTHREADS_STACK_SIZE;

	context->uc_link = NULL;

	return context;
}


