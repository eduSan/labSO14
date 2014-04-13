/**
* @file pcb.c
* @brief Function definitions for handling Process Control Blocks.
* @details The functions defined in this file are responsible for the allocation and allocation of ProcBlocks,
* 				 as well as for the creation and maintenance of process queues and process trees.
* @author Patrizia Thomas
* @author Eduardo Santarelli
* @version 0.1
* @date 2014-02-23
*/

#include "const.h"
#include "types.h"
#include "pcb.h"


HIDDEN pcb_t *pcbFree_h; /**< pointer to the head of a list of free ProcBlocks */

/* PCB allocation Functions */

/**
* @brief Initialize the pcbFree list
*
* Initialize the pcbFree list to contain all the elements of the
* static array of MAXPROC ProcBlk’s. This method will be called
* only once during data structure initialization.
*/
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


/**
* Insert the element pointed to by p onto the pcbFree list.
*
* @param *p The ProcBlock to be inserted.
*/
void freePcb(pcb_t *p){

	p->p_next = pcbFree_h;
	pcbFree_h = p;

}


/**
* @brief Allocate a new Process Control Block.
*
* Return NULL if the pcbFree list is empty. Otherwise, remove
* an element from the pcbFree list, provide initial values for ALL
* of the ProcBlk’s fields (i.e. NULL and/or 0) and then return a
* pointer to the removed element. ProcBlk’s get reused, so it is
* important that no previous value persist in a ProcBlk when it
* gets reallocated.
*
* @return A pointer to a ProcBlk with all its fields initialized,
* 				 or NULL if the pcbFree list is empty.
*/
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

/**
* @brief Initialize an empty Process Queue.
*
* This method is used to initialize a variable to be tail pointer to a
* process queue.
*
* @return A pointer to the tail of an empty process queue; i.e. NULL.
*/
pcb_t *mkEmptyProcQ(){

	pcb_t *tp = NULL;
	return tp;

}


/**
* Check if the queue whose tail is pointed to by tp is empty.
*
* @param *tp The pointer to the tail of a Process Queue.
* @return TRUE if the queue is empty, FALSE otherwise.
*/
int emptyProcQ(pcb_t *tp){

	if(tp == NULL)
		return TRUE;
	else
		return FALSE;

}


/**
* @brief Insert a new ProcBlk in a ProcQ.
*
* Insert the ProcBlk pointed to by p into the process queue whose
* tail-pointer is pointed to by tp. Note the double indirection through
* tp to allow for the possible updating of the tail pointer as well.
*
* @param **tp The address of a pointer to the tail of a Process Queue.
* @param *p A pointer to the pcb to be inserted into the queue.
*/
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


/**
* @brief Return the head of a ProcQ.
*
* Return a pointer to the first ProcBlk from the process queue whose
* tail is pointed to by tp. Do not remove this ProcBlkfrom the process
* queue. Return NULL if the process queue is empty.
* 
* @param *tp A pointer to the tail of a Process Queue.
*/
pcb_t *headProcQ(pcb_t *tp){

	if(tp)
		return tp->p_next;
	else
		return NULL;

}


/**
* @brief Dequeue a ProcBlk from a ProcQ.
*
* Remove the first (i.e. head) element from the process queue whose
* tail-pointer is pointed to by tp. Return NULL if the process queue
* was initially empty; otherwise return the pointer to the removed
* element. Update the process queue’s tail pointer if necessary.
*
* @param **tp The address of a pointer to the tail of a Process Queue.
* @return A pointer to the removed element, or NULL if the queue was empty.
*/
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


/**
* @brief Remove and return a specific ProcBlock from a ProcQ.
*
* Remove the ProcBlk pointed to by p from the process queue whose
* tail-pointer is pointed to by tp. Update the process queue’s tail
* pointer if necessary. If the desired entry is not in the indicated queue
* (an error condition), return NULL; otherwise, return p. Note that p
* can point to any element of the process queue.
* 
* @param **tp The address of a pointer to the tail of a Process Queue.
* @param *p A pointer to the pcb to be removed from the queue.
* @return A pointer to the removed ProcBlock, or NULL if an error occurs.
*/
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

/**
* Return TRUE if the ProcBlk pointed to by p has no children. Return FALSE otherwise.
*
* @param p The pointer to a ProcBlk.
* @retval TRUE
* @retval FALSE.
*/
int emptyChild(pcb_t *p){

	if(p->p_child == NULL)
		return TRUE;
	else
		return FALSE;

}


/**
* Make the ProcBlk pointed to by p a child of the ProcBlk pointed to by prnt.
*
* @param prnt A pointer to the ProcBlk that will become parent of *p
* @param p A pointer to the ProcBlk to be inserted as a child of *prnt.
*/
void insertChild(pcb_t *prnt, pcb_t *p){

	p->p_sib = prnt->p_child;
	prnt->p_child = p;
	p->p_prnt = prnt;

}


/**
* @brief Remove the first child of *p from his children.
*
* Make the first child of the ProcBlk pointed to by p no longer a
* child of p. Return NULL if initially there were no children of p.
* Otherwise, return a pointer to this removed first child ProcBlk.
*
* @param p A pointer to the ProcBlk whose first child is to be removed.
* @return A pointer to the removed first child of *p, or NULL if *p has no children.
*/

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



/**
* @brief Make the ProcBlk pointed to by p no longer the child of its parent.
*
* Make the ProcBlk pointed to by p no longer the child of its parent.
* If the ProcBlk pointed to by p has no parent, return NULL; otherwise,
* return p. Note that the element pointed to by p need not be the first
* child of its parent.
*
* @param p A pointer to the ProcBlock to be removed from its parent's list of children.
* @return A pointer to the removed ProcBlk, or NULL if *p had no parent.
*/
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
