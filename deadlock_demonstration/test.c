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
		char* line = NULL;
		char buf[1500];
		close(pipe1[0]);
		close(pipe2[1]);
		int nbytes;
		size_t nbytest = 0;

		if ((fdin = fopen("/usr/cs/fac1/bill/cs308/cs308a2_sort_data", "r")) == 0) {
			perror("failed to open file\n");
			exit(1);
		}
		
		while (getline(&line, &nbytest, fdin) != -1) {
			write(pipe1[1], line, nbytest);
			write(pipe1[1], "\n", 1);
		}
		
		/*
		while ((nbytes = fread(buf, 1, sizeof(buf), fdin)) > 0) {
			write(pipe1[1], buf, nbytes);
		}
		*/
		//free(line);
		fclose(fdin);
		close(pipe1[1]);
		
		// wait child
		wait(NULL);
		while ((nbytes = read(pipe2[0], buf, sizeof(buf))) > 0)
			printf("%.*s", nbytes, buf);
		return 0;
	}
	else if (childpid == 0) {
		close(pipe1[1]);
		close(pipe2[0]);
		dup2(pipe2[1], STDOUT_FILENO);
		dup2(pipe1[0], STDIN_FILENO);
		close(pipe2[1]);
		close(pipe1[0]);

		execlp("sort", "sort", (char *)0);

		fprintf(stderr, "failed to exec 'sort'\n");
		exit(1);
	}

}