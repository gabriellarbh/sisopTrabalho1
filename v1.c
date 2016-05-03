#include "cdata.h"
#include "fila2.h"
#include "cthread.h"
#include "libaux.h"
#define IDNULL 0
#define TRUE 1
#define FALSE 0

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

