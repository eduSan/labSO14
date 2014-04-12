#ifndef PCB_H
#define PCB_H

#include "const.h"
#include "types.h"

/* process control block type */

typedef struct pcb_t {
	/* Queue management */
	struct pcb_t *p_next;

	/* Process Tree management */
	struct pcb_t *p_prnt;
	struct pcb_t *p_child;
	struct pcb_t *p_sib;
	
	state_t p_s;	/* Processor state */
	int *p_semAdd;	/* Active semaphore Key */
} pcb_t;


/* PCB handling functions */

/* List view functions */

EXTERN void freePcb(pcb_t *p);
EXTERN pcb_t *allocPcb(void);
EXTERN void initPcbs(void);
EXTERN pcb_t *mkEmptyProcQ(void);
EXTERN int emptyProcQ(pcb_t *tp);
EXTERN void insertProcQ(pcb_t **tp, pcb_t *p);
EXTERN pcb_t *removeProcQ(pcb_t **tp);
EXTERN pcb_t *outProcQ(pcb_t **tp, pcb_t *p);
EXTERN pcb_t *headProcQ(pcb_t *tp);

/* Tree view functions */

EXTERN void initASL(void);
EXTERN int emptyChild(pcb_t *p);
EXTERN void insertChild(pcb_t *prnt, pcb_t *p);
EXTERN pcb_t *removeChild(pcb_t *p);
EXTERN pcb_t *outChild(pcb_t *p);

#endif
