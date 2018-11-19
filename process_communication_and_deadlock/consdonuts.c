#include "prodcons.h"

int		shmid, semid[3];

int main(int argc, char *argv[])
{
	int dozen_num = 0;
	int greatest = 0;
	int track_donut[NUMFLAVORS];
	int a_dozen[NUMFLAVORS][12];
	int count_12 = 0;
	int	i, j, k;
	char time_str[100];
	struct donut_ring *shared_ring;
	struct timeval randtime;

	for (i = 0; i < NUMFLAVORS; ++i) {
		track_donut[i] = 0;
		for (j = 0; j < 12; ++j) {
			a_dozen[i][j] = 0;
		}
	}

	if ((shmid = shmget(MEMKEY, sizeof(struct donut_ring), IPC_CREAT | 0600)) == -1) {
		perror("shared get failed: ");
		exit(1);
	}

	if ((shared_ring = (struct donut_ring *)shmat(shmid, NULL, 0)) == -1) {
		perror("shared attach failed: ");
		exit(1);
	}

	for (i = 0; i < NUMSEMIDS; i++) {
		if ((semid[i] = semget(SEMKEY + i, NUMFLAVORS, IPC_CREAT | 0600)) == -1) {
			perror("semaphore allocation failed: ");
			exit(1);
		}
	}
	

	gettimeofday(&randtime, (struct timezone *)0);
	unsigned short xsub1[3];
	xsub1[0] = (ushort)randtime.tv_usec;
	xsub1[1] = (ushort)(randtime.tv_usec >> 16);
	xsub1[2] = (ushort)(getpid());

	while (dozen_num < 10) {
		j = nrand48(xsub1) & 3;
		p(semid[CONSUMER], j);
		p(semid[OUTPTR], j);
		a_dozen[j][track_donut[j]] = shared_ring->flavor[j][shared_ring->outptr[j]];
		++track_donut[j];
		++count_12;
		if (count_12 == 12) {
			for (i = 0; i < NUMFLAVORS; ++i) {
				greatest = track_donut[i] > greatest ? track_donut[i] : greatest;
			}
			gettimeofday(&randtime, (struct timezone *)0);
			strftime(time_str, 100, "%H:%M:%S", localtime(&randtime.tv_sec));
			printf("Process ID: %d\t\ttime: %s.%03ld \t\tDozen #: %d\n", (int)getpid(), time_str, randtime.tv_usec/1000, dozen_num + 1);
			printf("plain\t\tjelly\t\tcoconut\t\thoney-dip\n");
			for (i = 0; i < greatest; ++i) {
				for (k = 0; k < NUMFLAVORS; ++k) {
					if (a_dozen[k][i] == 0) printf("\t\t");
					else printf("%d\t\t", a_dozen[k][i]);
					if (k == 3) {
						printf("\n");
						if (i == greatest - 1)
							printf("\n\n");
					}
					a_dozen[k][i] = 0;
				}
			}
			count_12 = 0;
			for (k = 0; k < NUMFLAVORS; ++k) {
				track_donut[k] = 0;
			}
			++dozen_num;
			greatest = 0;
		}
		shared_ring->outptr[j] = (shared_ring->outptr[j] + 1) % NUMSLOTS;
		v(semid[OUTPTR], j);
		v(semid[PROD], j);
	}
	exit(0);
}