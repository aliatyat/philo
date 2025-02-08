#ifndef PHILO_H
# define PHILO_H

# include <pthread.h>
# include <stdio.h>
# include <stdlib.h>
# include <sys/time.h>
# include <unistd.h>

typedef struct s_philosopher
{
    int             id;
    int             meals_eaten;
    long long       last_meal;
    pthread_mutex_t *left_fork;
    pthread_mutex_t *right_fork;
    struct s_data   *data;
    pthread_t       thread;
} t_philosopher;

typedef struct s_data
{
    int             num_philos;
    int             time_to_die;
    int             time_to_eat;
    int             time_to_sleep;
    int             must_eat_count;
    int             dead;
    int             finished_eating;
    long long       start_time;
    pthread_mutex_t dead_lock;
    pthread_mutex_t meal_lock;
    pthread_mutex_t *forks;
    pthread_mutex_t print_lock;
    t_philosopher   philosophers[250];
} t_data;

// Function prototypes
long long   get_time(void);
void        print_status(t_philosopher *philo, char *status);
void        take_forks(t_philosopher *philo);
void        put_forks(t_philosopher *philo);
void        *philosopher_routine(void *arg);
int         start_simulation(t_data *data);
int         init_philosophers(t_data *data);
int         validate_args(int argc, char **argv);
int         init_all(t_data *data, char **argv);
void        init_forks(t_data *data);
void        cleanup(t_data *data);
int         check_death_and_meals(t_philosopher *philo);

#endif