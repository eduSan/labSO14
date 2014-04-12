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

/**/
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


pcb_t *outBlocked(pcb_t *p){
/* Remove the ProcBlk pointed to by p from the process queue associated with
/p’s semaphore (p→ p_semAdd) on the ASL. If ProcBlk pointed to by p does not
/appear in the process queue associated with p’s semaphore, which is an error
/condition, return NULL; otherwise, return p. */
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


pcb_t *headBlocked(int *semAdd){
/* Return a pointer to the ProcBlk that is at the head of the process queue
/associated with the semaphore semAdd. Return NULL if semAdd is not found on the
/ASL or if the process queue associated with semAdd is empty. */
	semd_t *aux = semd_h; /* points to head of ASL */
	while (aux) {
		if (aux->s_semAdd == semAdd)
			return headProcQ(aux->s_procQ); /* i.e. either procQHead or NULL */
		aux = aux->s_next;				
	}
	return NULL;
}

