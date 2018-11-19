#include <sys/time.h>
#include <time.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/signal.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#define		SEMKEY			(key_t)510
#define		MEMKEY			(key_t)510
#define		NUMFLAVORS	 	4
#define		NUMSLOTS       	10
#define		NUMSEMIDS	 	3
#define		PROD		 	0
#define		CONSUMER	 	1
#define		OUTPTR		 	2

struct	donut_ring {
	int	flavor[NUMFLAVORS][NUMSLOTS];
	int	outptr[NUMFLAVORS];
};

union semun
{
	int val;
	struct semid_ds *buf;
	unsigned short int *array;
};

extern int		p(int, int);
extern int		v(int, int);
extern int		semsetall(int, int, int);