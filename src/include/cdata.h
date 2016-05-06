/*
 * cdata.h: arquivo de inclusão de uso apenas na geração da libpithread
 *
 * Esse arquivo pode ser modificado. ENTRETANTO, deve ser utilizada a TCB fornecida.
 *
 */
#include <ucontext.h>
#ifndef __cdata__
#define __cdata__

#define	PROCST_CRIACAO	0
#define	PROCST_APTO	1
#define	PROCST_EXEC	2
#define	PROCST_BLOQ	3
#define	PROCST_TERMINO	4

#define SUCCESS 0
#define ERROR -1

#define FALSE 0
#define TRUE 1

//tamanho da pilha para cada thread
 #define CTHREADS_STACK_SIZE 16384

/* NÃO ALTERAR ESSA struct */
typedef struct s_TCB { 
	int		tid; 		// identificador da thread
	int		state;		// estado em que a thread se encontra
					// 0: Criação; 1: Apto; 2: Execução; 3: Bloqueado e 4: Término
	ucontext_t 	context;	// contexto de execução da thread (SP, PC, GPRs e recursos) 
} TCB_t; 


typedef struct s_THREAD {
	//Thread a qual ESPERA por essa
	struct s_THREAD* waitedJoin;
	//Semaforo usado
	csem_t* semaforoUsado;
	//Usa a TCB do Cechin
	TCB_t* threadCB;
	//Ponteiro para a pilha do contexto
	char* stack;
} THREAD_t;

#endif
