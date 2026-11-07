#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_INPUT 1024
#define MAX_ARGS 64

/* max_args includes the final NULL slot; negative results indicate errors. */
static int lexer(char *input, char **args, int max_args)
{
  enum { NORMAL, IN_QUOTE } state = NORMAL;
  char *read = input;
  char *write = input;
  int argc = 0;
  int in_arg = 0;

  if (max_args <= 0) {
    return -1;
  }
  args[0] = NULL;

  while (*read != '\0') {
    char c = *read++;

    if (state == NORMAL && (c == ' ' || c == '\t')) {
      if (in_arg) {
        *write++ = '\0';
        in_arg = 0;
      }
      continue;
    }

    if (!in_arg) {
      if (argc == max_args - 1) {
        return -2;
      }
      args[argc++] = write;
      args[argc] = NULL;
      in_arg = 1;
    }

    if (c == '"') {
      state = state == NORMAL ? IN_QUOTE : NORMAL;
    } else {
      *write++ = c;
    }
  }

  *write = '\0';
  return state == IN_QUOTE ? -1 : argc;
}

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
    int argc;
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

    argc = lexer(input, args, MAX_ARGS);
    if (argc < 0) {
      fprintf(stderr, "parse error: %s\n",
              argc == -2 ? "too many arguments" : "unclosed double quote");
      continue;
    }
    if (argc == 0) {
      continue;
    }

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
