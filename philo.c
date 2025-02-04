#include "philo.h"

long long get_time(void)
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return ((tv.tv_sec * 1000LL) + (tv.tv_usec / 1000)); // Convert to milliseconds
}

void print_status(t_philosopher *philo, char *status)
{
    pthread_mutex_lock(&philo->data->print_lock);
    if (!philo->data->dead)
        printf("%lld Philosopher %d %s\n", get_time() - philo->data->start_time ,philo->id, status);
    pthread_mutex_unlock(&philo->data->print_lock);
}

void take_forks(t_philosopher *philo)
{
    if (philo->id % 2 == 0)
    {
        pthread_mutex_lock(philo->left_fork);
        print_status(philo, "took left fork");
        pthread_mutex_lock(philo->right_fork);
        print_status(philo, "took right fork");
    }
    else
    {
        pthread_mutex_lock(philo->right_fork);
        print_status(philo, "took right fork");
        pthread_mutex_lock(philo->left_fork);
        print_status(philo, "took left fork");
    }
}

void put_forks(t_philosopher *philo)
{
    pthread_mutex_unlock(philo->right_fork);
    pthread_mutex_unlock(philo->left_fork);
}

void *philosopher_routine(void *arg)
{
    t_philosopher *philo = (t_philosopher *)arg;
    //  if (philo->id % 2)
    //      usleep(15000);
    while (1)
    {
        pthread_mutex_lock(&philo->data->dead_lock);
        if (philo->data->dead || (philo->data->must_eat_count != -1 && philo->meals_eaten >= philo->data->must_eat_count))
        {
            pthread_mutex_unlock(&philo->data->dead_lock);
            break;
        }
        pthread_mutex_unlock(&philo->data->dead_lock);
        if(philo->data->num_philos == 1)
        {
            print_status(philo, "took right fork");
            usleep(philo->data->time_to_die * 1000);
             print_status(philo, "is died");
            return(0);
        }
        print_status(philo, "is thinking");
        take_forks(philo);
        print_status(philo, "is eating");
        pthread_mutex_lock(&philo->data->dead_lock);
        philo->last_meal = get_time();
        philo->meals_eaten++;
        pthread_mutex_unlock(&philo->data->dead_lock);
        //get_time();
        usleep(philo->data->time_to_eat * 1000);
        put_forks(philo);
        print_status(philo, "is sleeping");
        //get_time();
        usleep(philo->data->time_to_sleep * 1000);
    }
    return NULL;
}

// int monitor_philos(t_data *data, t_philosopher *philos)
// {
//     int i;

    
//     while (1)
//     {
//         i = 0;
//         while (i < data->num_philos)
//         {
//             pthread_mutex_lock(&data->dead_lock);
//             if ( ( get_time() - philos[i].last_meal) > data->time_to_die)
//             {
//                 print_status(&philos[i], "has died");
//                 data->dead = 1;
//                 pthread_mutex_unlock(&data->dead_lock);
//                 return (0);
//             }
//             pthread_mutex_unlock(&data->dead_lock);
//             i++;
//         }
//         usleep(1000);
//     }
// }

int start_simulation(t_data *data, t_philosopher *philos)
{
    int i;

    data->start_time = get_time();
    i = 0;
    while (i < data->num_philos)
    {
        if ( ( get_time() - philos[i].last_meal) > data->time_to_die)
         {
                 print_status(&philos[i], "has died");
                 data->dead = 1;
                 pthread_mutex_unlock(&data->dead_lock);
                 return (0);
        }
        if(pthread_create(&philos[i].thread, NULL, philosopher_routine, &philos[i]))
            return(0);
        pthread_mutex_lock(&philos->data->dead_lock);
        philos[i].last_meal = get_time();
        //printf("%lld, last meal\n", philos[i].last_meal);
        pthread_mutex_unlock(&philos->data->dead_lock);
        i++;
    }
    
   // monitor_philos(data, philos);
    return (0);
}

int init_philosophers(t_data *data, t_philosopher *philos)
{
    int i;

    i = 0;
    while (i < data->num_philos)
    {
        philos[i].id = i + 1;
        philos[i].meals_eaten = 0;
        philos[i].last_meal = get_time();
        philos[i].left_fork = &data->forks[i];
        philos[i].right_fork = &data->forks[(i + 1) % data->num_philos];
        philos[i].data = data;
        i++;
    }
    return 0;
}

int validate_args(int argc, char **argv)
{
    if (argc != 5 && argc != 6)
        return 0;
    if (atoi(argv[1]) <= 0 || atoi(argv[2]) <= 0 || atoi(argv[3]) <= 0 || atoi(argv[4]) <= 0)
        return 0;
    return 1;
}

int main(int argc, char **argv)
{
    if (!validate_args(argc, argv))
    {
        printf("Usage: ./philo num_philos time_to_die time_to_eat time_to_sleep [must_eat]\n");
        return 1;
    }

    t_data data;
    t_philosopher philos[200];  // Max philosophers: 200
    int i;

    data.num_philos = atoi(argv[1]);
    data.time_to_die = atoi(argv[2]);
    data.time_to_eat = atoi(argv[3]);
    data.time_to_sleep = atoi(argv[4]);

    if (argc == 6)
    {
        data.must_eat_count = atoi(argv[5]);
    }
    else
    {
        data.must_eat_count = -1;
    }

    data.dead = 0;

    pthread_mutex_init(&data.dead_lock, NULL);
    pthread_mutex_init(&data.print_lock, NULL);
    data.forks = malloc(sizeof(pthread_mutex_t) * data.num_philos);
    i = 0;
    while (i < data.num_philos)
    {
        pthread_mutex_init(&data.forks[i], NULL);
        i++;
    }
    init_philosophers(&data, philos);
    start_simulation(&data, philos);

    i = 0;
    while (i < data.num_philos)
    {
        pthread_join(philos[i].thread, NULL);
        i++;
    }
    i = 0;
    while (i < data.num_philos)
    {
        pthread_mutex_destroy(&data.forks[i]);
        i++;
    }
    pthread_mutex_destroy(&data.dead_lock);
    pthread_mutex_destroy(&data.print_lock);
    free(data.forks);

    return 0;
}