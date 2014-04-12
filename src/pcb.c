#include "const.h"
#include "types.h"
#include "pcb.h"


HIDDEN pcb_t *pcbFree_h; /* list of free ProcBlocks (head pointer)*/

/* PCB allocation Functions */

void initPcbs(){

	int i;
	HIDDEN pcb_t pcbFree_table[MAXPROC];
	pcb_t *tmp = NULL;

	/* Build a list out of the elements in pcbFree_table */
	pcbFree_h = pcbFree_table;
	tmp = pcbFree_h;

	for(i=1; i<MAXPROC; i++){
		tmp->p_next = pcbFree_table+i;
		tmp = tmp->p_next;
	}
	tmp->p_next = NULL; /* list is NULL-terminated */

}

/* insert an element to the head of freePcb list */
void freePcb(pcb_t *p){

	p->p_next = pcbFree_h;
	pcbFree_h = p;

}

/*get a procBlock from pcbFree (if present), then initialize and return it */
pcb_t *allocPcb(){

	if(!pcbFree_h)
		return NULL;
	else{
		pcb_t *tmp = NULL;
		tmp = pcbFree_h;
		pcbFree_h = tmp->p_next;
		
		tmp->p_next = NULL;
		tmp->p_prnt = NULL;
		tmp->p_child = NULL;
		tmp->p_sib = NULL;
		tmp->p_s = 0;
		tmp->p_semAdd = NULL;
		return tmp;
	}

}


/* Process Queues management */

/* create an empty procQ */
pcb_t *mkEmptyProcQ(){

	pcb_t *tp = NULL;
	return tp;

}

/* boolean: is the procQ pointed by tp empty? */
int emptyProcQ(pcb_t *tp){

	if(tp == NULL)
		return TRUE;
	else
		return FALSE;

}


/* Insert an element p in the procQ pointed by tp 	*
 * update tp if necessary				*/
void insertProcQ(pcb_t **tp, pcb_t *p){

	if(*tp == NULL){
		p->p_next = p;
		*tp = p;
	}
	else{
		p->p_next = (*tp)->p_next;
		(*tp)->p_next = p;
		(*tp) = p;
	}

}


/* return a pointer to the head of a queue */
pcb_t *headProcQ(pcb_t *tp){

	if(tp)
		return tp->p_next;
	else
		return NULL;

}

/* retrieve the first element of a queue, remove it from the queue */
pcb_t *removeProcQ(pcb_t **tp){

	pcb_t *tmp = NULL;

	if(*tp == NULL)		/* QUeue is empty */
		return NULL;
	else
		tmp = (*tp)->p_next;

	if(tmp == *tp)		/* If removing the only element in the */
		*tp = NULL; 	/* queue, must update the tail pointer */
	else
		(*tp)->p_next = tmp->p_next;

	return tmp;

}

pcb_t *outProcQ(pcb_t **tp, pcb_t *p){
	if (*tp == NULL)		/*if queue is empty return NULL*/
		return NULL;

	if ((*tp)->p_next == *tp) { 	/* queue only has one element.. */
		if (*tp == p) {		
			*tp = NULL;
			return p;	
		}
		else
			return NULL; /*..but not the element you're looking for*/
	}
	else {
		pcb_t *aux = *tp;
		/*scan the queue to determine if p is in the same queue *
		 *pointed by tp						*/
		while (aux->p_next != *tp) {
			if (aux->p_next == p) {
				aux->p_next = p->p_next;
				return p;
			}
			aux = aux->p_next;
		}
		if (aux->p_next == p) { /* aux points to second to last pcb in queue */
			aux->p_next = p->p_next; /* now aux is the queue tail */
			return p;
		}
		return NULL; /* queue doesn't contain p*/
	}		
}	

/* Process tree functions */

/* boolean: has node p got any child? */
int emptyChild(pcb_t *p){

	if(p->p_child == NULL)
		return TRUE;
	else
		return FALSE;

}

/*insert p as a child of prnt*/
void insertChild(pcb_t *prnt, pcb_t *p){

	p->p_sib = prnt->p_child;
	prnt->p_child = p;
	p->p_prnt = prnt;

}


/* remove the child of p (if present) */
pcb_t *removeChild(pcb_t *p){

	if(p->p_child == NULL) /*p has no child */
		return NULL;
	else{	/*remove the first child of p, return it (and its subtree) */
		pcb_t *tmp = NULL;
		tmp = p->p_child;
		p->p_child = tmp->p_sib;
		return tmp;
	}

}

/* p is no longer a child of its parent */
pcb_t* outChild(pcb_t *p){

	if(p->p_prnt == NULL)	/* p has no parent */
		return NULL;

	if((p->p_prnt)->p_child == p){		/*p is the first child */
		(p->p_prnt)->p_child = p->p_sib;
	}
	else{					/* p is NOT the first child */
		pcb_t *tmp = NULL;		/* I need to scan the list  */
		tmp = (p->p_prnt)->p_child;	/* of siblings for its */
						/* and update it */
		while(tmp->p_sib != p)
			tmp = tmp->p_sib;

		tmp->p_sib = p->p_sib;
	}	
	
	p->p_prnt = NULL;
	p->p_sib = NULL;
	return p;

}
