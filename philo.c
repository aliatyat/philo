#include "philo.h"

long long get_time(void)
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return ((tv.tv_sec * 1000LL) + (tv.tv_usec / 1000));
}

void *philosopher_routine(void *arg)
{
    t_philosopher *philo = (t_philosopher *)arg;

    while (1)
    {
        // Check if a philosopher has died
        pthread_mutex_lock(&philo->data->dead_lock);
        if (philo->data->dead)
        {
            pthread_mutex_unlock(&philo->data->dead_lock);
            break;
        }
        pthread_mutex_unlock(&philo->data->dead_lock);

        // Thinking
        printf("Philosopher %d is thinking $_$\n", philo->id);

        // Pick up forks
        pthread_mutex_lock(philo->left_fork);
        printf("Philosopher %d took left fork\n", philo->id);
        pthread_mutex_lock(philo->right_fork);
        printf("Philosopher %d took right fork\n", philo->id);

        // Eating
        printf("Philosopher %d is eating ^_^\n", philo->id);
        philo->last_meal = get_time();
        philo->meals_eaten++;
        usleep(philo->data->time_to_eat * 1000);

        // Put down forks
        pthread_mutex_unlock(philo->right_fork);
        pthread_mutex_unlock(philo->left_fork);

        // Sleeping
        printf("Philosopher %d is sleeping -_-\n", philo->id);
        usleep(philo->data->time_to_sleep * 1000);
    }
    return NULL;
}

void monitor_philosophers(t_data *data, t_philosopher *philos)
{
    int i;

    while (1)
    {
        i = 0;
        while (i < data->num_philos)
        {
            pthread_mutex_lock(&data->dead_lock);
            if (get_time() - philos[i].last_meal > data->time_to_die)
            {
                printf("Philosopher %d has died *_* \n", philos[i].id);
                data->dead = 1;
                pthread_mutex_unlock(&data->dead_lock);
                exit (1);
            }
            pthread_mutex_unlock(&data->dead_lock);
            i++;
        }
        usleep(1000);
    }
}

void start_simulation(t_data *data, t_philosopher *philos)
{
    int i = 0;

    data->start_time = get_time();
    while (i < data->num_philos)
    {
        if (pthread_create(&philos[i].thread, NULL, philosopher_routine, &philos[i]) != 0)
        {
            printf("Error: Failed to create thread for philosopher %d\n", i + 1);
            return;
        }
        i++;
    }
    monitor_philosophers(data, philos);
}

void cleanup(t_data *data, t_philosopher *philos)
{
    int i = 0;

    while (i < data->num_philos)
    {
        pthread_join(philos[i].thread, NULL);
        i++;
    }
    i = 0;
    while (i < data->num_philos)
    {
        pthread_mutex_destroy(&data->forks[i]);
        i++;
    }
    pthread_mutex_destroy(&data->dead_lock);
    free(data->forks);
}

int main(int argc, char **argv)
{
    t_data data;
    t_philosopher philos[200];
    int i = 0;

    if (argc != 5 && argc != 6)
        return (printf("Usage: ./philo num_philos time_to_die time_to_eat time_to_sleep [must_eat]\n"), 1);
    
    data.num_philos = atoi(argv[1]);
    data.time_to_die = atoi(argv[2]);
    data.time_to_eat = atoi(argv[3]);
    data.time_to_sleep = atoi(argv[4]);
    
    if (argc == 6)
        data.must_eat_count = atoi(argv[5]);
    else
        data.must_eat_count = -1;


        // Validate input arguments
    if (data.num_philos <= 0)
        return (printf("Error: Number of philosophers must be greater than 0\n"), 1);
    if (data.time_to_die <= 0 || data.time_to_eat <= 0 || data.time_to_sleep <= 0)
        return (printf("Error: Time values must be greater than 0\n"), 1);
    if (argc == 6 && data.must_eat_count <= 0)
        return (printf("Error: must_eat_count must be greater than 0\n"), 1);


    data.dead = 0; // Initialize dead flag
    data.stop_simulation = 0; // Initialize stop flag

    pthread_mutex_init(&data.dead_lock, NULL);
    data.forks = malloc(sizeof(pthread_mutex_t) * data.num_philos);
    if (!data.forks)
        return (printf("Error: Failed to allocate memory for forks\n"), 1);
    
    while (i < data.num_philos)
    {
        pthread_mutex_init(&data.forks[i], NULL);
        i++;
    }

    i = 0;
    while (i < data.num_philos)
    {
        philos[i].id = i + 1;
        philos[i].meals_eaten = 0;
        philos[i].last_meal = get_time();
        philos[i].left_fork = &data.forks[i];
        philos[i].right_fork = &data.forks[(i + 1) % data.num_philos];
        philos[i].data = &data;
        i++;
    }

    start_simulation(&data, philos);
    cleanup(&data, philos);
    return 0;
}