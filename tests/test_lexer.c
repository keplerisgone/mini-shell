#include <assert.h>

#define main mini_shell_main
#include "../main.c"
#undef main

static void check(const char *source, int expected_argc,
                  const char **expected)
{
  char input[MAX_INPUT];
  char *args[MAX_ARGS];

  strcpy(input, source);
  assert(lexer(input, args, MAX_ARGS) == expected_argc);
  for (int i = 0; i < expected_argc; i++) {
    assert(args[i] >= input && args[i] < input + sizeof(input));
    assert(strcmp(args[i], expected[i]) == 0);
  }
  assert(args[expected_argc] == NULL);
}

int main(void)
{
  const char *quoted[] = { "echo", "hello world" };
  const char *single[] = { "echo hello world" };
  const char *separate[] = { "echo", "hello", "world" };
  const char *empty[] = { "echo", "", "ab", "cd" };
  char input[MAX_INPUT];
  char *args[MAX_ARGS];

  check("echo \"hello world\"", 2, quoted);
  check("\"echo hello world\"", 1, single);
  check("\"echo\" \"hello\" \"world\"", 3, separate);
  check("echo hello\" world\"", 2, quoted);
  check(" \techo\t\"hello world\" \t", 2, quoted);
  check("echo \"\" a\"\"b \"c\"d", 4, empty);
  check("", 0, NULL);
  check(" \t ", 0, NULL);

  strcpy(input, "echo \"hello world");
  assert(lexer(input, args, MAX_ARGS) == -1);

  for (int i = 0; i < MAX_ARGS; i++) {
    input[2 * i] = 'x';
    input[2 * i + 1] = ' ';
  }
  input[2 * (MAX_ARGS - 1)] = '\0';
  assert(lexer(input, args, MAX_ARGS) == MAX_ARGS - 1);
  assert(args[MAX_ARGS - 1] == NULL);
  input[2 * (MAX_ARGS - 1)] = 'x';
  input[2 * (MAX_ARGS - 1) + 1] = '\0';
  /* Restore separators overwritten by the previous in-place parse. */
  for (int i = 0; i < MAX_ARGS - 1; i++) {
    input[2 * i + 1] = ' ';
  }
  assert(lexer(input, args, MAX_ARGS) == -2);
  assert(args[MAX_ARGS - 1] == NULL);
  return 0;
}
