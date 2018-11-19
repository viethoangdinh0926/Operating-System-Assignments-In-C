#include <signal.h>
#include <sys/time.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>



#define         NUMFLAVORS       	4
#define         NUMSLOTS        	1500
#define			NUMCONSUMERS     	50
#define			NUMPRODUCERS		30
#define			NUMDOZENS			200

struct  donut_ring {
	int     	flavor[NUMFLAVORS][NUMSLOTS];
	int     	outptr[NUMFLAVORS];
	int			in_ptr[NUMFLAVORS];
	int			serial[NUMFLAVORS];
	int			spaces[NUMFLAVORS];
	int			donuts[NUMFLAVORS];
};

/**********************************************************************/
/* SIGNAL WAITER, PRODUCER AND CONSUMER THREAD FUNCTIONS              */
/**********************************************************************/

void	*sig_waiter(void *arg);
void	*producer(void *arg);
void	*consumer(void *arg);
void    sig_handler(int);
