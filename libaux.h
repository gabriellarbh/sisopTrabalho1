/*

					Interface da Biblioteca auxiliar da GaVini <3   v1.0.0.0
					Tem que revisar :)
					Falta a função escalonar();!
	
*/
#include "cdata.h"
#include "fila2.h"
#include "cthread.h"
#include "libaux.h"
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



