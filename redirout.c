#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#define MAX_BUF 1024

int main(int argc, char *argv[]) {
  int fd_out, fd_pipe[2];
  pid_t pid;
  char buffer[MAX_BUF];
  ssize_t bytes_read;

  if (argc < 3) {
    printf("Usage: %s outfilename command opt1 opt2 ...\n", argv[0]);
    exit(1);
  }

  fd_out = open(argv[1], O_CREAT | O_TRUNC | O_WRONLY, 0644);
  if (fd_out == -1) {
    perror("Error opening output file");
    exit(1);
  }

  if (pipe(fd_pipe) == -1) {
    perror("Error creating pipe");
    close(fd_out);
    exit(1);
  }

  pid = fork();
  if (pid == -1) {
    perror("Error forking process");
    close(fd_out);
    close(fd_pipe[0]);
    close(fd_pipe[1]);
    exit(1);
  }

  if (pid == 0) {
    close(fd_out);

    dup2(fd_pipe[1], STDOUT_FILENO);
    close(fd_pipe[0]);

    execvp(argv[2], &argv[2]);
    perror("Error executing command");
    close(fd_pipe[1]);
    exit(1);
  } else {
    close(fd_pipe[1]);

    while ((bytes_read = read(fd_pipe[0], buffer, MAX_BUF)) > 0) {
      write(fd_out, buffer, bytes_read);
    }

    close(fd_pipe[0]);
    close(fd_out);
  }

  return 0;
}
