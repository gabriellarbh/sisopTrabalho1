/*
 * cdata.h: arquivo de inclus�o de uso apenas na gera��o da libpithread
 *
 * Esse arquivo pode ser modificado. ENTRETANTO, deve ser utilizada a TCB fornecida.
 *
 */
#ifndef __cdata__
#define __cdata__

#define	PROCST_CRIACAO	0
#define	PROCST_APTO	1
#define	PROCST_EXEC	2
#define	PROCST_BLOQ	3
#define	PROCST_TERMINO	4

#define SUCCESS 0
#define ERROR -1

/* N�O ALTERAR ESSA struct */
typedef struct s_TCB { 
	int		tid; 		// identificador da thread
	int		state;		// estado em que a thread se encontra
					// 0: Cria��o; 1: Apto; 2: Execu��o; 3: Bloqueado e 4: T�rmino
	ucontext_t 	context;	// contexto de execu��o da thread (SP, PC, GPRs e recursos) 
} TCB_t; 

typedef struct s_THREAD {
	//id da thread que essa est� esperando
	int idEsperado;
	//se usa recurso, n�o sei se vai ser mantida
	int usaRecurso;
	//ID da thread que est� esperando
	int esperadoPor;
	//Semaforo usado
	csem_t* semaforoUsado;
	//Usa a TCB do Cechin
	TCB_t threadCB;
}THREAD_t;

#endif
