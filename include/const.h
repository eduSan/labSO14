/**
* @file const.h
* @brief Constant definitions for Kaya.
*/
#ifndef CONST_H
#define CONST_H

/* Kaya specific constants */

/**
* Max number of overall (eg, system, daemons, user)
* concurrent processes.
*/
#define MAXPROC 20

/**
* number of usermode processes (not including maste
* proc and system daemons.
*/
#define UPROCMAX 3  


/* general purpose constants */
#define EXTERN extern
#define HIDDEN static
#define FALSE 0
#define TRUE 1

#define NULL ((void *)0)

#endif
