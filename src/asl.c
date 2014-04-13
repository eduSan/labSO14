/**
* @file asl.c
* @brief Function definitions for handling the Active Semaphore List
* @details This file contains the implementation of the Active Semaphore List data structure and the necessary functions to allocate semaphores and manage their associated ProcQs.
* @author Patrizia Thomas
* @author Eduardo Santarelli
* @version 0.1
* @date 2014-02-23
*/
#include "asl.h" 
#include "const.h"
#include "libuarm.h"

/* semaphore descriptor type */ 
typedef struct semd_t { 
	struct semd_t *s_next; /* next element on the ASL */ 
	int *s_semAdd;         /* pointer to the semaphore */ 
	pcb_t *s_procQ;        /* tail pointer to a process queue */
} semd_t;
 

HIDDEN semd_t *semd_h, /* head of the ASL */
	      *semdFree_h; /* head of the semdFree list */


/* ASL functions */

/* Initialize the semdFree list to contain all the elements of the array
static semd t semdTable[MAXPROC] */
void initASL(void){ 
	int i;	
	static semd_t semdTable[MAXPROC+1]; /*I declare an extra element to use as a dummy node */
	
	/*build a list out of the elements in semdTable */
	semdFree_h = &(semdTable[0]);
	for (i = 0; i < MAXPROC-1; i++)
		semdTable[i].s_next = &(semdTable[i+1]);			
	semdTable[i].s_next = NULL;	
	semd_h = &semdTable[MAXPROC]; /*Keep the last item as the head(dummy node)*/
}


void initSemd(void){ 
	semd_h->s_next = NULL;		 /*DUMMY NODE. s_next field points to first ASL entry */
}


/**
* @brief Insert a ProcBlk in the ProcQ associated with a specified semaphore.
*
* Insert the ProcBlk pointed to by p at the tail of the process queue
* associated with the semaphore whose physical address is semAdd
* and set the semaphore address of p to semAdd. If the semaphore is
* currently not active (i.e. there is no descriptor for it in the ASL),
* allocate a new descriptor from the semdFree list, insert it in the ASL (at
* the appropriate position), initialize all of the fields (i.e. set s semAdd
* to semAdd, and s procq to mkEmptyProcQ()), and proceed as
* above. If a new semaphore descriptor needs to be allocated and the
* semdFree list is empty, return TRUE. In all other cases return FALSE.
*
* @param semAdd The address of a semaphore.
* @param p A pointer to a ProcBlk.
*
* @retval TRUE A new semaphore descriptor needs to be allocated, but none are avalaible.
* @retval FALSE The ProcBlk has been successfully inserted in a queue.
*/
int insertBlocked(int *semAdd, pcb_t *p){
	semd_t *current = semd_h->s_next;
	semd_t *prev = semd_h;

	/* scan the ASL until the end, or until an element has a bigger semAdd*/
	while(current != NULL && current->s_semAdd < semAdd){
		current = current->s_next;
		prev = prev->s_next;
	}

	/* if the correct ASL entry exists, it must be *current update the queue */
	if(current != NULL && current->s_semAdd == semAdd){
		insertProcQ(&(current->s_procQ), p);
		p->p_semAdd = semAdd;
		return FALSE;
	}
	/* if not, insert the element before the current one ... */
	else{
		if(semdFree_h == NULL){	  /*unless we are out sem descriptors */
			return TRUE;
		}
		else{
			semd_t *newSemd = NULL;
			newSemd = semdFree_h;
			semdFree_h = newSemd->s_next;
			
			prev->s_next = newSemd;
			newSemd->s_next = current;
	
			newSemd->s_semAdd = semAdd;
			newSemd->s_procQ = mkEmptyProcQ();
			insertProcQ(&(newSemd->s_procQ), p);
			p->p_semAdd = semAdd;
			return FALSE;
		}
	}
}


/**
* @brief Dequeue a ProcBlk from the ProcQ of the found semaphore descriptor.
*
* Search the ASL for a descriptor of this semaphore. If none is
* found, return NULL; otherwise, remove the first (i.e. head) ProcBlk
* from the process queue of the found semaphore descriptor and re-
* turn a pointer to it. If the process queue for this semaphore becomes
* empty (emptyProcQ(s procq) is TRUE), remove the semaphore
* descriptor from the ASL and return it to the semdFree list.
*
* @param semAdd The address of a sempahore descriptor.
*
* @return A pointer to the removed ProcBlk.
* @return NULL if no descriptor for semAdd is found in the ASL.
*/
pcb_t *removeBlocked(int *semAdd){

	semd_t* current = semd_h->s_next;
	semd_t* prev = semd_h;
	pcb_t *removed = NULL;

	/* scan the list until the end, or until we find our entry */
	while(current != NULL && current->s_semAdd != semAdd){
		current = current->s_next;
		prev = prev->s_next;
	}

	/*current is either NULL, or has the correct semAdd*/
	if(current  == NULL)
		return NULL;
	else{
		removed = removeProcQ(&(current->s_procQ));
	/*if we removed the last procBlk, the semaphore must be deactivated*/
		if(emptyProcQ(current->s_procQ)){
			prev->s_next = current->s_next;
			current->s_next = semdFree_h;
			semdFree_h = current;
		}
		return removed;
	}
}


/**
* @brief Remove an arbitrary ProcBlk from the ProcQ associated with its semaphore.
*
* Remove the ProcBlk pointed to by p from the process queue associated
* with p’s semaphore (p->p_semAdd) on the ASL. If ProcBlk
* pointed to by p does not appear in the process queue associated with
* p’s semaphore, which is an error condition, return NULL; otherwise,
* return p.
*
* @param p A pointer to the ProcBlk to be removed.
*
* @return The removed ProcBlk.
* @return NULL if an error occurred.
*/
pcb_t *outBlocked(pcb_t *p){
	if (!p)
		return NULL;
	semd_t *aux = semd_h; /* points to head of ASL */
	while (aux) {
		if (aux->s_semAdd == p->p_semAdd)
			return outProcQ(&(aux->s_procQ), p); /* i.e. either p or NULL */
		aux = aux->s_next;							
	}
	return NULL;
}


/**
* @brief Return a pointer to the head of the ProcQ associated with a specified semaphore.
*
* Return a pointer to the ProcBlk that is at the head of
* the process queue associated with the semaphore semAdd.
* Return NULL if semAdd is not found on the ASL or if the
* process queue associated with semAdd is empty.
*
* @param semAdd The address of a semaphore.
*
* @return A pointer to the head of the queue associated with *semAdd.
* @return NULL The ProcQ associated with *semAdd is empty.
*/
pcb_t *headBlocked(int *semAdd){
	semd_t *aux = semd_h; /* points to head of ASL */
	while (aux) {
		if (aux->s_semAdd == semAdd)
			return headProcQ(aux->s_procQ); /* i.e. either procQHead or NULL */
		aux = aux->s_next;				
	}
	return NULL;
}

