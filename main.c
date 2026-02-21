#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>
#include "ai_module.h"

#define LSH_RL_BUFSIZE 1024
#define LSH_TOK_BUFSIZE 64
#define LSH_TOK_DELIM " \t\r\n\a"

/* Forward Declarations */
int lsh_cd(char **args);
int lsh_help(char **args);
int lsh_exit(char **args);

/* Built-in Command Registry */
static char *builtin_str[] = {
    "cd",
    "help",
    "exit",
    "suggest"
};

static int (*builtin_func[])(char **) = {
    &lsh_cd,
    &lsh_help,
    &lsh_exit,
    &lsh_suggest
};

static int lsh_num_builtins() {
    return sizeof(builtin_str) / sizeof(char *);
}


void sigint_handler(int sig) {
    char cwd[1024];
    (void)sig; /* Suppress unused variable warning */

    printf("\n");
    
    /* Fetch the current directory dynamically inside the signal handler */
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        /* Print the full, colored prompt */
        printf("\033[1;36mkaiVen\033[0m:\033[1;32m%s\033[0m > ", cwd);
    } else {
        /* Fallback if it fails */
        printf("kaiVen > ");
    }
    
    fflush(stdout); /* Force the terminal to print immediately */
}

/* --- Built-in Implementations --- */

int lsh_cd(char **args) {
    if (args[1] == NULL) {
        fprintf(stderr, "kaiVen: cd: missing operand\n");
    } else {
        if (chdir(args[1]) != 0) {
            perror("kaiVen: cd");
        }
    }
    return 1;
}

int lsh_help(char **args) {
    (void)args;
    printf("kaiVen shell, version 1.0\n");
    printf("These shell commands are defined internally. Type `help' to see this list.\n");
    for (int i = 0; i < lsh_num_builtins(); i++) {
        printf(" %s\n", builtin_str[i]);
    }
    return 1;
}

int lsh_exit(char **args) {
    (void)args;
    return 0;
}

/* --- Process Management --- */

int lsh_launch(char **args) {
    pid_t pid;
    int status;
    int i;
    int fd;

    pid = fork();
    if (pid == 0) {
        /* Child Process: Reset signal handler to default for executed programs */
        signal(SIGINT, SIG_DFL);

        /* Handle Output Redirection */
        for (i = 0; args[i] != NULL; i++) {
            if (strcmp(args[i], ">") == 0) {
                if (args[i+1] == NULL) {
                    fprintf(stderr, "kaiVen: syntax error near unexpected token `newline'\n");
                    exit(EXIT_FAILURE);
                }
                
                fd = open(args[i+1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
                if (fd < 0) {
                    perror("kaiVen");
                    exit(EXIT_FAILURE);
                }

                dup2(fd, STDOUT_FILENO);
                close(fd);
                args[i] = NULL;
                break;
            }
        }

        if (execvp(args[0], args) == -1) {
            perror("kaiVen");
        }
        exit(EXIT_FAILURE);
    } else if (pid < 0) {
        perror("kaiVen: fork");
    } else {
        /* Parent Process: Wait for child termination */
        do {
            waitpid(pid, &status, WUNTRACED);
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }

    return 1;
}

int lsh_execute(char **args) {
    if (args[0] == NULL) {
        return 1; /* Empty command */
    }

    for (int i = 0; i < lsh_num_builtins(); i++) {
        if (strcmp(args[0], builtin_str[i]) == 0) {
            return (*builtin_func[i])(args);
        }
    }

    return lsh_launch(args);
}

/* --- Input Parsing --- */

char *lsh_read_line(void) {
    char *line = NULL;
    size_t bufsize = 0;

    if (getline(&line, &bufsize, stdin) == -1) {
        if (feof(stdin)) {
            printf("\n");
            exit(EXIT_SUCCESS);
        } else {
            perror("kaiVen: getline");
            exit(EXIT_FAILURE);
        }
    }
    return line;
}

char **lsh_split_line(char *line) {
    int bufsize = LSH_TOK_BUFSIZE, position = 0;
    char **tokens = malloc(bufsize * sizeof(char *));
    char *token;

    if (!tokens) {
        fprintf(stderr, "kaiVen: memory allocation failed\n");
        exit(EXIT_FAILURE);
    }

    token = strtok(line, LSH_TOK_DELIM);
    while (token != NULL) {
        tokens[position] = token;
        position++;

        if (position >= bufsize) {
            bufsize += LSH_TOK_BUFSIZE;
            tokens = realloc(tokens, bufsize * sizeof(char *));
            if (!tokens) {
                fprintf(stderr, "kaiVen: memory allocation failed\n");
                exit(EXIT_FAILURE);
            }
        }

        token = strtok(NULL, LSH_TOK_DELIM);
    }
    tokens[position] = NULL;
    return tokens;
}

/* --- Main Shell Loop --- */

void lsh_loop(void) {
    char *line;
    char **args;
    int status;
    char cwd[1024];

    do {
        if (getcwd(cwd, sizeof(cwd)) != NULL) {
            /* Minimalist, standard UNIX prompt */
            printf("\033[1;36mkaiVen\033[0m:\033[1;32m%s\033[0m$ ", cwd);
        } else {
            perror("kaiVen: getcwd");
            printf("kaiVen$ ");
        }
        
        fflush(stdout);
        
        line = lsh_read_line();
        args = lsh_split_line(line);
        status = lsh_execute(args);

        free(line);
        free(args);
    } while (status);
}

int main(int argc, char **argv) {
    (void)argc;
    (void)argv;
    
    signal(SIGINT, sigint_handler);
    lsh_loop();
    
    return EXIT_SUCCESS;
}