#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main(void)
{
  pid_t pid = fork();
  if (pid == -1) {
    perror("fork");
    return 1;
  }

  if (pid == 0) {
    printf("Child process\n");
    char *const argv[] = {
      "ls",
      "-l",
      NULL
    };
    execvp("ls", argv);
    perror("execvp");
    return 1;
  }

  printf("Parent process\n");
  int status;
  waitpid(pid, &status, 0);

  return 0;
}
