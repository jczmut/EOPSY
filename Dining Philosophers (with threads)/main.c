#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>

// EOPSY task 7
// Dining Philosophers problem - solution made using threads

// ADDITIONAL QUESTIONS

/*  1) Would it be sufficient just to add to the old algorithm from task5 additional mutex variable to organize critical sections in functions grab_forks() and put_away_forks()
    or making changes to values of two mutexes indivisably?

    No, it would not be sufficient, because the mutexes are local to the process and they would not work between the processes (and previous algorithm included processes). 

    A mutex is a locking mechanism; only one thread can aquire it and release it.
    A semaphore is a signaling mechanism; one task signals that it is done and that another one can start.
    



   2) Why m mutex is initializes with 1 and mutexes from the array s are initialized with 0's?

    Mutex m is used to manage the critical sections for the forks, so when a philosopher wants to either grab them, or put them back. It is initialized as 1 (is unlocked) so that a philosopher can reach for them.
    If it was 0 at the beginning, no philosopher could have started eating and all of them would starve.
    The mutexes from the s array are used to manage the threads execution, so when the philosophers can change their states.
    If they were 1's at the beginning, all philosophers could have tried to reach for the forks at the same time and there would be a deadlock.
    The philosopher's mutex is unlocked when he is able to get the resources (forks).
*/

#define NUM_PHILOSOPHERS 5                                          // number of the philosophers
#define LEFT (NUM_PHILOSOPHERS + id - 1) % NUM_PHILOSOPHERS         // id of the philosopher on the left
#define RIGHT (id + 1) % NUM_PHILOSOPHERS                           // id of the philosopher on the right
#define NUM_MEALS 5                                                 // number of meals in the simulation
#define MAX_PROGRAM_RUNTIME 60

enum philosopher_state {
    Eating,     // by default 0
    Thinking,   // 1
    Hungry      // 2
};

// mutexes
pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;                          // forks mutex
pthread_mutex_t s[NUM_PHILOSOPHERS] = PTHREAD_MUTEX_INITIALIZER;        // philosophers' mutexes

enum philosopher_state state[NUM_PHILOSOPHERS];                         // array for each philosopher's state


void *start_philosopher(void *arg);
void grab_forks(int left_fork_id);
void check_if_can_eat(int id);
void put_away_forks(int left_fork_id);

void print_eating(int philosopher);
void print_thinking(int philosopher);
void print_hungry(int philosopher);
void print_finished_meal(int philosopher, int meal);


int main() {

    // creating array of threads representing the philosophers
    pthread_t philosophers[NUM_PHILOSOPHERS];

    // creating array with IDs of the philosophers
    int philosophers_IDs[NUM_PHILOSOPHERS];


    // initializing mutexes
    pthread_mutex_unlock(&m);    // unlocking forks mutex (initializing as 1)

    // philosophers' mutexes
    for(int i=0; i<NUM_PHILOSOPHERS; i++) {
        // initializing the mutex
        if(pthread_mutex_init(&s[i], NULL) != 0) {      // attr parameter = NULL, so default mutex attributes are used
            perror("main(): Error while creating philosophers mutexes.\n");
            return 1;
        }
        
        pthread_mutex_lock(&s[i]);      // locking each philosopher's mutex as the starting state (initializing as 0)
    }

    // initializing the philosophers threads
    for(int i=0; i<NUM_PHILOSOPHERS; i++) {

        // initializing the state array
        state[i] = Thinking;   // Thinking state is the default one

        // initializing the philosophers' ID
        philosophers_IDs[i] = i;

        // creating the thread
        if(pthread_create(philosophers + i, NULL, start_philosopher, (void *)(philosophers_IDs + i)) != 0) {
            // it starts a new thread by calling start_philosopher routine with its argument (last argument here)
            // attr = NULL, so defaults are chosen
            perror("main(): Error while creating the threads.\n");
            return 1;
        }
    }

    // wait for the specified time (MAX_PROGRAM_RUNTIME) while the threads run
    sleep(MAX_PROGRAM_RUNTIME);

    // by this point, all threads should finish execution, so we can destroy them and the mutexes
    
    // kill all philosophers threads
    for(int i=0; i<NUM_PHILOSOPHERS; i++) {

        // cancelling thread's execution
        pthread_cancel(philosophers[i]);

        // waiting for this thread to terminate
        if(pthread_join(philosophers[i], NULL) != 0) {      // if the thread has terminated already, the function returns immediately; retval = NULL
            perror("main(): Error while terminating the threads.\n");
            return 1;
        }
    }

    // destroying the mutexes
    pthread_mutex_destroy(&m);      // destroying the forks mutex
    
    // destroying the philosophers' mutexes
    for(int i=0; i< NUM_PHILOSOPHERS; i++) {
        pthread_mutex_destroy(&s[i]);
    }

    return 0;    
}

// __________________________________________ FUNCTIONS __________________________________________


// function for the threads (philosophers) to take the resources (forks)
void grab_forks(int left_fork_id) {

    int id = left_fork_id;

    // forks critical section
    // down(&m)
    pthread_mutex_lock(&m);

    // state changing - the philosopher wants to eat
    state[id] = Hungry;

    // philosopher is trying to eat
    check_if_can_eat(id);

    // up(&m)
    pthread_mutex_unlock(&m);

    // down(&s[id])
    pthread_mutex_lock(&s[id]);

}

// function for the threads (philosophers) to give up the resources (forks)
void put_away_forks(int left_fork_id) {
    
    int id = left_fork_id;

    // fork critical section
    // down(&m)
    pthread_mutex_lock(&m);

    // the philosopher stopped eating and will start thinking
    state[id] = Thinking;

    // checking if neighbors want and can eat
    check_if_can_eat(LEFT);
    check_if_can_eat(RIGHT);

    // up(&m)
    pthread_mutex_unlock(&m);

}

// function for checking if a philosopher wants to and can eat
void check_if_can_eat(int id) {

    // if philosopher is hungry and the neighbors are not eating, then he will be able to start eating
    if(state[id] == Hungry && state[LEFT] != Eating && state[RIGHT] != Eating) {
        // set the state to Eating
        state[id] = Eating;

        // up(&s[id])
        pthread_mutex_unlock(&s[id]);
    }

}

// start routine for pthread_create function
void *start_philosopher(void *arg) {

    int *id = (int *)arg;

    // loop for the sequence of all meals
    for(int i=0; i< NUM_MEALS; i++) {
        print_thinking(*id);
        grab_forks(*id);
        print_eating(*id);
        print_finished_meal(*id, i+1);
        put_away_forks(*id);
    }
    // after finishing all the meals, philosopher starts thinking
    printf("Philosopher %d: Finished all %d meals and is thinking...\n", *id, NUM_MEALS);
    
    return NULL;
}

void print_eating(int philosopher) {
    int sleep_time = 1;
    //printf("Philosopher %d: Eating...\n", philosopher);
    sleep(sleep_time);
}

void print_thinking(int philosopher) {
    int sleep_time = 1;
    //printf("Philosopher %d: Thinking...\n", philosopher);
    sleep(sleep_time);
}

void print_finished_meal(int philosopher, int meal) {
    printf("Philosopher %d: Finished meal number %d...\n", philosopher, meal);
}
