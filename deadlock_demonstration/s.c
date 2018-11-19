#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(void) {
	int pipe1[2];
	int pipe2[2];
	pid_t childpid;
	FILE *fdin;

	pipe(pipe1);
	pipe(pipe2);

	// error handling
	if ((childpid = fork()) == -1) {
		perror("fork");
		exit(1);
	}

	// parent load file, write to pipe1
	if (childpid != 0) {

		char buf[1024], area_str[5];
		close(pipe1[0]);
		close(pipe2[1]);
		int nbytes;

		if ((fdin = fopen("/usr/cs/fac1/bill/cs308/cs308a2_sort_data", "r")) == 0) {
			perror("failed to open file\n");
			exit(1);
		}

		while ((nbytes = fread(buf, 1, sizeof(buf), fdin)) > 0) {
			write(pipe1[1], buf, nbytes);
		}
		fclose(fdin);	
		close(pipe1[1]);

		//shoud not wait for child to die

		dup2(pipe2[0], 0);
		execlp("awk", "awk", "{A[$3]++}END{for(i in A)print \"area code:\",i,\"occurs\",A[i],\"times\"}", (char *)0);
		perror("execlp fail.");
		exit(1);
	}
	else if (childpid == 0) {//child process
		close(pipe1[1]);
		close(pipe2[0]);
		dup2(pipe2[1], STDOUT_FILENO);
		dup2(pipe1[0], STDIN_FILENO);
		close(pipe2[1]);
		close(pipe1[0]);

		execlp("sort", "sort", "-k3,3n", "-k1,1", (char *)0);

		perror("execlp fail.");
		exit(1);
	}
	
}