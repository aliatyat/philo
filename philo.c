#include "philo.h"


long long get_time(void)
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return ((tv.tv_sec * 1000LL) + (tv.tv_usec / 1000)); // Convert to milliseconds
}

void *philosopher_routine(void *arg)
{
    t_philosopher *philo = (t_philosopher *)arg;

    while (1)
    {
        // Think
        printf("Philosopher %d is thinking 🤔\n", philo->id);

        // Take forks (lock mutexes)
        pthread_mutex_lock(philo->left_fork);
        printf("Philosopher %d took left fork 🍴\n", philo->id);
        pthread_mutex_lock(philo->right_fork);
        printf("Philosopher %d took right fork 🍴\n", philo->id);

        // Eat
        printf("Philosopher %d is eating 😋\n", philo->id);
        philo->last_meal = get_time();
        philo->meals_eaten++;
        usleep(philo->data->time_to_eat * 1000);

        // Put down forks (unlock mutexes)
        pthread_mutex_unlock(philo->right_fork);
        pthread_mutex_unlock(philo->left_fork);

        // Sleep
        printf("Philosopher %d is sleeping 😴\n", philo->id);
        usleep(philo->data->time_to_sleep * 1000);
    }
    return NULL;
}


void start_simulation(t_data *data, t_philosopher *philos)
{
    int i;

    // Get the start time
    data->start_time = get_time();
    
    // Create philosopher threads
    for (i = 0; i < data->num_philos; i++)
        pthread_create(&philos[i].thread, NULL, philosopher_routine, &philos[i]);

    // Monitor if any philosopher dies
    while (1)
    {
        for (i = 0; i < data->num_philos; i++)
       // i = 0;
        //while(i < data->num_philos)
        {
            pthread_mutex_lock(&data->dead_lock);
            if (get_time() - philos[i].last_meal > data->time_to_die)
            {
                printf("💀 Philosopher %d has died\n", philos[i].id);
                data->dead = 1;
               
            }
            pthread_mutex_unlock(&data->dead_lock);
            if (data->dead)
                return;
           // i++;
        }
        usleep(1000);
    }
}



int main(int argc, char **argv)
{
    if (argc != 5 && argc != 6)
        return (printf("Usage: ./philo num_philos time_to_die time_to_eat time_to_sleep [must_eat]\n"), 1);

    t_data data;
    t_philosopher philos[200];  // Max philosophers: 200
    int i;

    // Parse arguments
    data.num_philos = atoi(argv[1]);
    data.time_to_die = atoi(argv[2]);
    data.time_to_eat = atoi(argv[3]);
    data.time_to_sleep = atoi(argv[4]);
    data.must_eat_count = (argc == 6) ? atoi(argv[5]) : -1;

    // Initialize mutexes
    pthread_mutex_init(&data.dead_lock, NULL);
    data.forks = malloc(sizeof(pthread_mutex_t) * data.num_philos);
    for (i = 0; i < data.num_philos; i++)
    //i = 0;
   // while(i < data.num_philos)
    {
        pthread_mutex_init(&data.forks[i], NULL);
    i++;
    }
    // Initialize philosophers
    //for (i = 0; i < data.num_philos; i++)
    i = 0;
    while(i <data.num_philos)
    {
        philos[i].id = i + 1;
        philos[i].meals_eaten = 0;
        philos[i].last_meal = get_time();
        philos[i].left_fork = &data.forks[i];
        philos[i].right_fork = &data.forks[(i + 1) % data.num_philos];
        philos[i].data = &data;
        i++;
    }

    // Start simulation
    start_simulation(&data, philos);

    // Cleanup
   for (i = 0; i < data.num_philos; i++)
   //i = 0;
   //while(i < data.num_philos)
        pthread_join(philos[i].thread, NULL);
    //i++;
   for (i = 0; i < data.num_philos; i++)
   //while(i < data.num_philos)
        pthread_mutex_destroy(&data.forks[i]);
    pthread_mutex_destroy(&data.dead_lock);
    //i++;
    free(data.forks);


    return 0;
}
