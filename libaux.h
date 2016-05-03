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



