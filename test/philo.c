#include "philo.h"

int write_error(char *str)
{
    int len = 0;
    while (str[len])
        len++;
    write(2, "Error: ", 7);
    write(2, str, len);
    write(2, "\n", 1);
    return (1);
}

int error_manager(int error)
{
    if (error == 1)
        return (write_error("At least one wrong argument"));
    if (error == 2)
        return (write_error("Fatal error when initializing mutex"));
    return (1);
}

int init_mutex(t_data *data)
{
    int i = 0;
    while (i < data->num_philos)
    {
        if (pthread_mutex_init(&data->forks[i], NULL))
            return (1);
        i++;
    }
    if (pthread_mutex_init(&data->dead_lock, NULL))
        return (1);
    if (pthread_mutex_init(&data->print_lock, NULL))
        return (1);
    if (pthread_mutex_init(&data->meal_lock, NULL))
        return (1);
    return (0);
}

int init_philosophers(t_data *data)
{
    int i = 0;
    while (i < data->num_philos)
    {
        data->philosophers[i].id = i + 1;
        data->philosophers[i].meals_eaten = 0;
        data->philosophers[i].last_meal = get_time();
        data->philosophers[i].left_fork = &data->forks[i];
        data->philosophers[i].right_fork = &data->forks[(i + 1) % data->num_philos];
        data->philosophers[i].data = data;
        i++;
    }
    return (0);
}

int init_all(t_data *data, char **argv)
{
    data->num_philos = atoi(argv[1]);
    data->time_to_die = atoi(argv[2]);
    data->time_to_eat = atoi(argv[3]);
    data->time_to_sleep = atoi(argv[4]);
    data->must_eat_count = (argv[5]) ? atoi(argv[5]) : -1;
    data->dead = 0;
    data->finished_eating = 0;
    data->start_time = get_time();

    if (data->num_philos <= 0 || data->time_to_die <= 0 || data->time_to_eat <= 0
        || data->time_to_sleep <= 0 || data->num_philos > 250)
        return (1);

    data->forks = malloc(sizeof(pthread_mutex_t) * data->num_philos);
    if (!data->forks)
        return (2);

    if (init_mutex(data))
        return (2);

    init_philosophers(data);
    return (0);
}

int main(int argc, char **argv)
{
    t_data data;
    int ret;

    if (argc != 5 && argc != 6)
        return (write_error("Wrong amount of arguments"));

    ret = init_all(&data, argv);
    if (ret)
        return (error_manager(ret));

    if (start_simulation(&data))
        return (write_error("There was an error creating the threads"));

    cleanup(&data);
    return (0);
}