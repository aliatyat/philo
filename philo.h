#ifndef PHILO_H
# define PHILO_H

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>


typedef struct s_data
{
    int         num_philos;        // Number of philosophers
    int         time_to_die;       // Time before a philosopher dies (ms)
    int         time_to_eat;       // Time spent eating (ms)
    int         time_to_sleep;     // Time spent sleeping (ms)
    int         must_eat_count;    // Optional: Number of times each philosopher must eat
    long long   start_time;        // Simulation start time
    int         dead;              // 1 if a philosopher has died
    pthread_mutex_t dead_lock;     // Protects the 'dead' variable
    pthread_mutex_t *forks;        // Array of mutexes for forks
    int stop_simulation;
    pthread_mutex_t stop_mutex;
} t_data;


typedef struct s_philosopher
{
    int             id;         // Philosopher ID (1 to num_philos)
    int             meals_eaten;// Count of meals eaten
    long long       last_meal;  // Timestamp of last meal
    pthread_t       thread;     // Thread for the philosopher
    pthread_mutex_t *left_fork; // Pointer to left fork mutex
    pthread_mutex_t *right_fork;// Pointer to right fork mutex
    t_data          *data;      // Pointer to shared data
} t_philosopher;



int main(int argc, char **argv);










#endif