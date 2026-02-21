// main.c
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include "ai_module.h" // NEW: We import the menu for our AI tools!

// --- Defines ---
#define LSH_RL_BUFSIZE 1024
#define LSH_TOK_BUFSIZE 64
#define LSH_TOK_DELIM " \t\r\n\a"

// --- Forward Declarations ---
int lsh_cd(char **args);
int lsh_help(char **args);
int lsh_exit(char **args);

// --- Built-in Command List ---
char *builtin_str[] = {
  "cd",
  "help",
  "exit",
  "suggest" // Our AI command is still registered here
};

int (*builtin_func[]) (char **) = {
  &lsh_cd,
  &lsh_help,
  &lsh_exit,
  &lsh_suggest // Points to the function inside ai_module.c
};

int lsh_num_builtins() {
  return sizeof(builtin_str) / sizeof(char *);
}

// --- Built-in Function Implementations ---
int lsh_cd(char **args)
{
  if (args[1] == NULL) {
    fprintf(stderr, "karnVen: expected argument to \"cd\"\n");
  } else {
    if (chdir(args[1]) != 0) {
      perror("karnVen");
    }
  }
  return 1;
}

int lsh_help(char **args)
{
  int i;
  printf("Welcome to karnVen-sh! 🚀\n");
  printf("Type program names and arguments, and hit enter.\n");
  printf("The following are built in:\n");

  for (i = 0; i < lsh_num_builtins(); i++) {
    printf("  %s\n", builtin_str[i]);
  }

  printf("Use the man command for information on other programs.\n");
  return 1;
}

int lsh_exit(char **args)
{
  return 0; // Returning 0 breaks the loop and closes the shell
}

// --- Process Execution & Redirection ---
int lsh_launch(char **args)
{
  pid_t pid;
  int status;
  int i;
  int fd;

  pid = fork();
  if (pid == 0) {
    // Child process: Check for Redirection (>)
    for (i = 0; args[i] != NULL; i++) {
      if (strcmp(args[i], ">") == 0) {
        if (args[i+1] == NULL) {
          fprintf(stderr, "karnVen: expected filename after '>'\n");
          exit(EXIT_FAILURE);
        }
        
        fd = open(args[i+1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (fd < 0) {
          perror("karnVen");
          exit(EXIT_FAILURE);
        }

        dup2(fd, 1); // The Hijack!
        close(fd);
        args[i] = NULL; 
        break; 
      }
    }

    if (execvp(args[0], args) == -1) {
      perror("karnVen");
    }
    exit(EXIT_FAILURE);
  } else if (pid < 0) {
    perror("karnVen");
  } else {
    // Parent process
    do {
      waitpid(pid, &status, WUNTRACED);
    } while (!WIFEXITED(status) && !WIFSIGNALED(status));
  }

  return 1;
}

int lsh_execute(char **args)
{
  int i;

  if (args[0] == NULL) {
    return 1; // Empty command
  }

  // Check if the command is a built-in (cd, help, exit, suggest)
  for (i = 0; i < lsh_num_builtins(); i++) {
    if (strcmp(args[0], builtin_str[i]) == 0) {
      return (*builtin_func[i])(args);
    }
  }

  // If not a built-in, try to launch it as a Linux program (ls, grep, etc.)
  return lsh_launch(args);
}

// --- Input Reading & Parsing ---
char *lsh_read_line(void)
{
  char *line = NULL;
  size_t bufsize = 0; 

  if (getline(&line, &bufsize, stdin) == -1){
    if (feof(stdin)) {
      exit(EXIT_SUCCESS); // End of file (Ctrl+D)
    } else  {
      perror("readline");
      exit(EXIT_FAILURE);
    }
  }
  return line;
}

char **lsh_split_line(char *line)
{
  int bufsize = LSH_TOK_BUFSIZE, position = 0;
  char **tokens = malloc(bufsize * sizeof(char*));
  char *token;

  if (!tokens) {
    fprintf(stderr, "karnVen: allocation error\n");
    exit(EXIT_FAILURE);
  }

  token = strtok(line, LSH_TOK_DELIM);
  while (token != NULL) {
    tokens[position] = token;
    position++;

    if (position >= bufsize) {
      bufsize += LSH_TOK_BUFSIZE;
      tokens = realloc(tokens, bufsize * sizeof(char*));
      if (!tokens) {
        fprintf(stderr, "karnVen: allocation error\n");
        exit(EXIT_FAILURE);
      }
    }

    token = strtok(NULL, LSH_TOK_DELIM);
  }
  tokens[position] = NULL;
  return tokens;
}

// --- The Core Loop ---
void lsh_loop(void)
{
  char *line;
  char **args;
  int status;

  do {
    printf("karnVen 🚀 > ");
    line = lsh_read_line();
    args = lsh_split_line(line);
    status = lsh_execute(args);

    free(line);
    free(args);
  } while (status);
}

// --- Entry Point ---
int main(int argc, char **argv)
{
  lsh_loop();
  return EXIT_SUCCESS;
}