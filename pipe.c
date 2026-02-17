// Save this as: pipe_test.c
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

#define MSGSIZE 16
char* msg1 = "hello, world #1";
char* msg2 = "hello, world #2";
char* msg3 = "hello, world #3";

int main() {
    char inbuf[MSGSIZE];
    int p[2], pid, nbytes;

    // 1. Create Pipe
    if (pipe(p) < 0)
        exit(1);

    // 2. Fork Process
    if ((pid = fork()) > 0) {
        // --- PARENT (Writer) ---
        
        // Parent does not need to read, so close the Read End immediately.
        close(p[0]); 

        write(p[1], msg1, MSGSIZE);
        write(p[1], msg2, MSGSIZE);
        write(p[1], msg3, MSGSIZE);

        // Parent is done writing. CLOSE the Write End.
        // This sends the "EOF" signal to the child.
        close(p[1]); 
        
        wait(NULL); // Wait for child to finish
    }
    else {
        // --- CHILD (Reader) ---
        
        // Child does not need to write, so close the Write End immediately.
        close(p[1]);

        // Keep reading until EOF (which happens when Parent closes p[1])
        while ((nbytes = read(p[0], inbuf, MSGSIZE)) > 0)
            printf("%s\n", inbuf);
            
        if (nbytes != 0)
            exit(2);
            
        printf("Finished reading\n");
        close(p[0]); // Good practice to close read end when done
    }
    return 0;
}