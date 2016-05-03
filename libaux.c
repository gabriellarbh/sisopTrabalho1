/*

					Implementação da Biblioteca auxiliar da GaVini <3   v1.0.0.0
					Tem que revisar :)
	
*/

//Pega a thread atual na lista de aptos 
THREAD_t* current_thread() {
	THREAD_t* p;
	if(!setIterator(lstApto, threadAtualID))
		return (THREAD_t*) GetAtIterator2(lstApto);
	return NULL
}	

void cBlock() {
	//Pega a thread atual e seta seu estado para bloqueado.
	THREAD_t *aux = current_thread();
	if(aux != NULL)
		aux->threadCB->state = PROCST_BLOQ;
}



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

//Seta o iterador da fila em um dado ID
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



//Dada uma fila, retorna o tamanho da mesma
int sizeFila(PFILA2 fila){

	PNODE2 aux;
	int retorno = 0;
	for(aux = fila->first; aux != NULL; aux = aux->next)
		retorno++;
	return retorno;
}
//Termina a thread atual
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
//Seta o novo id da thread
int get_new_id()
{
	int id = global_thread_id;
	++global_thread_id;
	return id;
}

void create_main_thread(void* context)
{
	ucontext_t* main_context = (ucontext_t*) context;
	*(current_thread()->context) = *main_context;

	/* retorna para main */
	return (void) setcontext(current_thread()->context);
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
        allocator_context.uc_link = &current_context;
		/* seta que allocator_context começa na função allocate_thread_private */
        makecontext(&allocator_context,
			(void(*)(void))allocate_thread_private, 0);

        setcontext(&allocator_context);
    }

    return allocator_buffer;
}

//Dá free em um contexto
void free_context(ucontext_t* context){
	if (context != NULL){
		free(context->uc_stack.ss_sp);
		free(context);
	}
}


//Aloca um novo contexto
ucontext_t* allocate_context()
{
	ucontext_t* context = (ucontext_t*) malloc (sizeof(ucontext_t));

	if (getcontext(context) != 0 || context == NULL)
		return NULL;

	//aloca a pilha do contexto
	context->uc_stack.ss_sp = (char*) malloc(MTHREADS_STACK_SIZE);
	context->uc_stack.ss_size = MTHREADS_STACK_SIZE;

	context->uc_link = NULL;

	return context;
}

