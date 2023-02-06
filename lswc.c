#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
int pipefd[2];
pid_t pid;
char *directory;
if (argc > 1) {
    directory = argv[1];
} else {
    directory = ".";
}

if (pipe(pipefd) == -1) {
    perror("pipe");
    exit(EXIT_FAILURE);
}

pid = fork();
if (pid == -1) {
    perror("fork");
    exit(EXIT_FAILURE);
}

if (pid == 0) { // Child process
    dup2(pipefd[0], STDIN_FILENO);
    close(pipefd[1]);
    execlp("wc", "wc", "-l", (char *)NULL);
} else { // Parent process
    dup2(pipefd[1], STDOUT_FILENO);
    close(pipefd[0]);
    execlp("ls", "ls", "-1a", directory, (char *)NULL);
}

return 0;
}