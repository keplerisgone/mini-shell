#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_ARGS 64

int main(void)
{
  char *line = NULL;
  size_t line_size = 0;

  while (1) {
    char *argv[MAX_ARGS];
    int argc = 0;

    printf("main> ");
    fflush(stdout);

    if (getline(&line, &line_size, stdin) == -1) {
      break;
    }

    char *token = strtok(line, " \t\n");
    while (token != NULL && argc < MAX_ARGS - 1) {
      argv[argc++] = token;
      token = strtok(NULL, " \t\n");
    }
    argv[argc] = NULL;

    if (argc == 0) {
      continue;
    }

    pid_t pid = fork();
    if (pid == -1) {
      perror("fork");
      free(line);
      return 1;
    }

    if (pid == 0) {
      execvp(argv[0], argv);
      perror("execvp");
      free(line);
      return 1;
    }

    int status;
    if (waitpid(pid, &status, 0) == -1) {
      perror("waitpid");
      free(line);
      return 1;
    }
  }

  free(line);

  return 0;
}
