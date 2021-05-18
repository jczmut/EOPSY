// semaphores let processes query or alter status information; they are often used to monitor and control 
//      the availability of system resources such as shared memory segments
//      a semaphore must be initialized using semget, operations are performed using semop

// *******************************************************************************************
// Implementation of the Dining Philosophers problem in C language
// Author: Julia Czmut 300168

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/types.h>  // data types
#include <sys/ipc.h>    // interprocess communication access structure
#include <sys/sem.h>    // semaphores
#include <sys/wait.h>
#include <sys/shm.h>    // shared memory
#include <time.h>
#include <signal.h>


#define NUM_PHILOSOPHERS 5                                          // number of the philosophers
#define LEFT (NUM_PHILOSOPHERS + id - 1) % NUM_PHILOSOPHERS         // id of the philosopher on the left
#define RIGHT (id + 1) % NUM_PHILOSOPHERS                           // id of the philosopher on the right
#define NUM_MEALS 3                                                 // number of meals in the simulation
#define MAX_PROGRAM_RUNTIME 60

enum philosopher_state {
    Eating,     // by default 0
    Thinking,   // 1
    Hungry      // 2
};

// array of the philosophers' states (shared by all processes)
struct shared_memory_segment {
    enum philosopher_state states[NUM_PHILOSOPHERS];
} *shm_segment;

// ********************************** FUNCTIONS **********************************


// PHILOSOPHERS' FUNCTIONS
void print_eating(int philosopher) {
    int sleep_time = 1;
    printf("Philosopher %d: Eating...\n", philosopher, sleep_time);
    sleep(sleep_time);
}

void print_thinking(int philosopher) {
    int sleep_time = 0;
    printf("Philosopher %d: Thinking...\n", philosopher, sleep_time);
    sleep(sleep_time);
}

void print_hungry(int philosopher) {
    printf("Philosopher %d: Being hungry and trying to grab the forks...\n", philosopher);
}

void print_finished_meal(int philosopher) {
    printf("Philosopher %d: Finished the meal and getting ready for some thinking...\n", philosopher);
}

// function for toggling the semaphore
void lock_semaphore(int semid, int bufid, bool lock) {      // if lock is false, wa want to unlock the semaphore; if true - lock it
    int buf_change = -1;    // -1 locks

    if(!lock) {
        buf_change = +1;    // +1 unlocks
    }
    struct sembuf sem_change = {bufid, buf_change, SEM_UNDO};       // {semaphore number, semaphore operation, operation flag} 

    if(semop(semid, &sem_change, 1) < 0) {
        perror("lock_semaphore: Error during semaphore lock.\n");
        exit(1);
    }
    // if sem_op (in our case buf_change) is positive, the operation adds this value to the semaphore value. If additionally SEM_UNDO is specified, sem_op value is subtracted from semaphore adjustment value and the operation can proceed - the thread does not have to wait.
    // if sem_op is negative, process has to alter permission on the semaphore set. If SEM_UNDO is specified, abs(sem_op) is added to semaphore adjustment value. Counter of threads waiting for this semaphore's value to increase (semncnt) is incremented and the thread sleeps.
}

// function for checking if a philosopher can eat (so if there are free forks beside him)
void check_if_can_eat(int id) {
    int philosopher_semaphore = semget(0x2000, 1, 0660);    // nsems=1, because it's for one philosopher

    // checking for an error
    if(philosopher_semaphore < 0) {
        perror("check_if_can_eat(): Error while getting the semaphore.");
        exit(1);
    }

    // checking states of himself and the neighbors to determine whether he can eat
    if(shm_segment->states[id] == Hungry && !(shm_segment->states[LEFT] == Eating) && !(shm_segment->states[RIGHT] == Eating)) {
        // the philosopher has to be hungry and his neighbors cannot be eating at this moment
        shm_segment->states[id] = Eating;       // if above conditions are met, changing the state from Hungry to Eating
        lock_semaphore(philosopher_semaphore, id, false);   // unlock the semaphore for this philosopher, because he is starting to eat
    }

}

//=======================================================================================================================
// function for grabbing the forks
void grab_forks(int left_fork_id) {

    int id = left_fork_id;      // initialization needed for defined macros LEFT and RIGHT
    
    // getting the semaphores -> semget(key_t key, int nsems, int semflg)
    int philosophers_semaphores = semget(0x2000, NUM_PHILOSOPHERS, 0660);  // 0660 specifies permissions -rw-rw----
    int forks_semaphores = semget(0x3000, 1, 0660);

    // checking if semaphores were created successfully
    if(philosophers_semaphores < 0 || forks_semaphores < 0) {
        perror("grab_forks(): Error while getting the semaphores.\n");
        exit(1);
    }

    // starting the exchange of forks
    lock_semaphore(forks_semaphores, 0, true);      // lock the forks semaphore for the time of checking if there are forks beside the philosopher

    // the philosopher becomes hungry
    shm_segment->states[id] = Hungry;
    print_hungry(id);

    // the philosopher checks if he can eat
    check_if_can_eat(id);

    // ending the exchange of forks - updating the state of the semaphore
    lock_semaphore(forks_semaphores, 0, false);     
    // updating the state of the semaphore for philosophers - the philosopher is eating
    lock_semaphore(philosophers_semaphores, id, true);

}

//=======================================================================================================================
// function for putting away the forks (giving way for others to eat)
void put_away_forks(int left_fork_id) {

    int id = left_fork_id;      // initialization needed for defined macros LEFT and RIGHT

    // getting the semaphores -> semget(key_t key, int nsems, int semflg)
    int philosophers_semaphores = semget(0x2000, NUM_PHILOSOPHERS, 0660);  // 0660 specifies explicit permissions
    int forks_semaphores = semget(0x3000, 1, 0660);

    // checking if semaphores were created successfully
    if(philosophers_semaphores < 0 || forks_semaphores < 0) {
        perror("put_away_forks(): Error while getting the semaphores.\n");
        exit(1);
    }

    // starting the exchange of forks - lock the semaphore
    lock_semaphore(forks_semaphores, 0, true);

    // the philosopher stops eating and goes back to the state thinking
    shm_segment->states[id] = Thinking;
    print_finished_meal(id);
    print_thinking(id);

    // checking whether the philosophers' neighbors can eat and if they can, they do
    check_if_can_eat(LEFT);
    check_if_can_eat(RIGHT);

    // ending the exchange of forks - updating the state of the semaphore
    lock_semaphore(forks_semaphores, 0, false);     
}
//=======================================================================================================================

// function for killing all philosophers processes
void kill_all_philosophers(pid_t *processes, int num_philosophers) {
    for(int i=0; i<num_philosophers; i++) {
        kill(processes[i], SIGTERM);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int main() {

    // creating the shared memory consisting of the array with philosophers' states
    int shm = shmget(0x1000, sizeof(struct shared_memory_segment), 0644 | IPC_CREAT);       // permissions 0644 rw-r--r--
    // if value -1, then something went wrong
    if(shm == -1) {
        perror("main(): Error while creating the shared memory.\n");
        return 1;
    }

    // attaching shared memory to the shared memory segment
    shm_segment = shmat(shm, NULL, 0);
    // if successful, shmat() returns the address of attached memory segment; if not - it returns (void *)-1
    if(shm_segment == (void *)-1) {
        perror("main(): Error while attaching the shared memory.\n");
        return 1;
    }

    // getting the forks semaphore
    int forks_semaphore = semget(0x3000, 1, 0666 | IPC_CREAT);      // permissions 0666 -rw-rw-rw-
    // if semaphore < 0, something went wrong
    if(forks_semaphore < 0) {
        perror("main(): Error while creating forks semaphore.\n");
        return 1;
    }

    // defining a union needed for semctl() command which performs the control operation
    union semun {
        int val;                // value for SETVAL
        unsigned short *array;  // array for GETALL, SETALL
    } sem_un;
    sem_un.val = 1;

    // performing the control operation on the forks semaphore
    if (semctl(forks_semaphore, 0, SETVAL, sem_un) < 0) {   // semctl returns int
        perror("main(): Error while performing the control operation on the forks semaphore.\n");
        return 1;
    }

    // getting the philosophers semaphores
    int philosophers_semaphores = semget(0x2000, NUM_PHILOSOPHERS, 0666 | IPC_CREAT);
    if(philosophers_semaphores < 0) {
        perror("main(): Error while creating the philosophers' semaphores.\n");
        return 1;
    }

    // control operation on philosophers semaphores
    unsigned short zeros[NUM_PHILOSOPHERS];     // array of zeros - places for the philosophers
    for(int i=0; i<NUM_PHILOSOPHERS; i++) {
        zeros[i] = 0;
    }
    sem_un.array = zeros;       // assigning the zeros array to the array component of the semaphore union

    // checking if control operation was successful
    if(semctl(philosophers_semaphores, 0, SETALL, sem_un) < 0) {        // SETALL = set all semaphores' semval
        perror("main(): Error while performing the control operation on the philosophers semaphores.\n");
        return 1;
    }

    // initializing all philosophers by bringing them to Thinking state
    for(int i=0; i<NUM_PHILOSOPHERS; i++) {
        shm_segment->states[i] = Thinking;
    }

    // main loop for the philosophers simulation
    pid_t philosophers_process_IDs[NUM_PHILOSOPHERS];       // array of the philosophers' IDs
    
    for(int i=0; i<NUM_PHILOSOPHERS; i++) {

        pid_t pid = fork();     // creating a process

        // checking for an error in creating the process
        if(pid < 0) {
            printf("main loop: Error while creating philosopher process number %d\n", i);
            kill_all_philosophers(philosophers_process_IDs, (i+1));     // kill all processes before exiting the program
            return 1;
        }

        // if pid > 0, we returned to parent process
        if(pid > 0) {
            philosophers_process_IDs[i] = pid;
            continue;           // skip to the next iteration
        }

        // else, pid = 0, so we are in child process
        for(int j=0; j<NUM_MEALS; j++) {    // iterating over the sequences of thinking, getting hungry, eating and putting down the forks
            print_thinking(i);
            grab_forks(i);
            print_eating(i);
            put_away_forks(i);
        }
       return 0;
    }

    // running the program until the max runtime
    sleep(MAX_PROGRAM_RUNTIME);
    kill_all_philosophers(philosophers_process_IDs, NUM_PHILOSOPHERS);  // kill all processes

    return 0;
}




