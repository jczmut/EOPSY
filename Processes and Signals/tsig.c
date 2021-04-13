// EOPSY task 2: tsig
// Author: Julia Czmut

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/wait.h>

#define WITH_SIGNALS    // indicator of the version of the algorithm: with or without signal modifications
#define NUM_CHILD 6     // number of child processes to create

short interrupted = 0;  // global variable indicating whether process was interrupted (1 if yes, 0 if not)


// handler of the SIGTERM signal, which provides information about termination of this process
void terminateHandler() {
    printf("child[%d]: Received SIGTERM signal, terminating process.\n", getpid());
}

// process performed by all children
void childProcess() {

    // if WITH_SIGNALS is defined
    #ifdef WITH_SIGNALS
        
        // ignore handling of the keyboard interrupt signal
        signal(SIGINT, SIG_IGN);

        // set the custom handler of termination
        signal(SIGTERM, terminateHandler);

    #endif

    printf("child[%d]: My parent id is: %d\n", getpid(), getppid());

    sleep(10);  // delay specified by the task

    printf("child[%d]: Execution completed.\n", getpid());
    exit(0);    // 0 indicates returning from a child process
}

// function for killing all child processes
void killChildren(int numChild, int* children) {
    for (int i=0; i<numChild; i++) {
        kill(children[i], SIGTERM);
    }
}

// handler of the SIGINT signal
void keyboardInterruptHandler() {
    printf("parent[%d]: Received a keyboard interrupt.\n", getpid());
    interrupted = 1;    // set this global variable to 1, because interrupt occurred
}

// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int main() {

    // if WITH_SIGNALS is defined
    #ifdef WITH_SIGNALS

        // ignore all signals
        for (int i=1; i<_NSIG; i++) {
            signal(i, SIG_IGN);
        }

        // restore SIGCHLD to default
        signal(SIGCHLD, SIG_DFL);

        // handle SIGINT with the custom interrupt handler
        signal(SIGINT, keyboardInterruptHandler);

    #endif

    pid_t childrenIDs[NUM_CHILD];   // array of children's ID's

    // the main big loop
    for (int i=0; i<NUM_CHILD; i++) {
        
        pid_t id = fork();    // create a child process

        if (id == 0) {   // so if returned to newly created child

            childProcess();

        } else if (id < 0) {    // unsuccessful creation

            printf("parent[%d]: Error while creating the child processes - killing all children and terminating the program.\n", getpid());
            killChildren(i, childrenIDs);

        } else {
            childrenIDs[i] = id;
        }

        sleep(1);   // delay between consecutive fork() calls specified by the task

        // if WITH_SIGNALS is defined
        #ifdef WITH_SIGNALS

            // make the parent kill the children if a keyboard interrupt occurred
            if (interrupted == 1) {
                printf("parent[%d]: Sending SIGTERM signal to all children...\n", getpid());
                killChildren(i + 1, childrenIDs);   // call the function for killing all child processes
                break;
            }

        #endif

    }

    if (interrupted == 1) {     // interrupt occurred
        printf("parent[%d]: Child process creation was interrupted.\n", getpid());
    }
    else {    // there was no interrupts
        printf("parent[%d]: All child processes were successfully created.\n", getpid());
    }

    // count the child process terminations and print the number of their exit codes
    int counter = 0;
    while (1) {     // infinite loop, we'll break when no more child processes are left
        if (wait(NULL) == -1) {     // if no more child processes, exit the loop
        printf("No more child processes.\n");
                break;
        }
        counter++;
    }
    printf("parent[%d]: Received %d child exit codes.\n", getpid(), counter);

    // restore old service handlers to default
    #ifdef WITH_SIGNALS

        for (int i=1; i< _NSIG; i++) {
            signal(i, SIG_DFL);
        }

    #endif

    return 0;
}
