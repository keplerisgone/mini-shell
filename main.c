#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_INPUT 1024
#define MAX_ARGS 64

static int handle_builtin(char **args)
{
  if (args[0] == NULL) {
    return 1;
  }

  if (strcmp(args[0], "cd") == 0) {
    char *path = args[1];

    if (path == NULL) {
      path = getenv("HOME");
      if (path == NULL) {
        fprintf(stderr, "cd: HOME not set\n");
        return 1;
      }
    }

    if (chdir(path) == -1) {
      perror("cd");
    }

    return 1;
  }

  if (strcmp(args[0], "pwd") == 0) {
    char cwd[4096];

    if (getcwd(cwd, sizeof(cwd)) == NULL) {
      perror("pwd");
    } else {
      printf("%s\n", cwd);
    }

    return 1;
  }

  if (strcmp(args[0], "exit") == 0) {
    return -1;
  }

  return 0;
}

int main(void)
{
  char input[MAX_INPUT];

  while (1) {
    char *args[MAX_ARGS];
    int argc = 0;
    int builtin_result;
    pid_t pid;
    int status;

    printf("mini-shell> ");
    fflush(stdout);

    if (fgets(input, sizeof(input), stdin) == NULL) {
      printf("\n");
      break;
    }

    input[strcspn(input, "\n")] = '\0';

    char *token = strtok(input, " \t");
    while (token != NULL && argc < MAX_ARGS - 1) {
      args[argc++] = token;
      token = strtok(NULL, " \t");
    }
    args[argc] = NULL;

    builtin_result = handle_builtin(args);
    if (builtin_result == 1) {
      continue;
    }
    if (builtin_result == -1) {
      break;
    }

    pid = fork();
    if (pid == -1) {
      perror("fork");
      continue;
    }

    if (pid == 0) {
      execvp(args[0], args);
      perror("execvp");
      return 1;
    }

    if (waitpid(pid, &status, 0) == -1) {
      perror("waitpid");
    }
  }

  return 0;
}
