#include "philo.h"

// Convert to milliseconds
long long	get_time(void)
{
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	return ((tv.tv_sec * 1000LL) + (tv.tv_usec / 1000));
}

void	print_status(t_philosopher *philo, char *status)
{
	pthread_mutex_lock(&philo->data->print_lock);
	if (!philo->data->dead)
		printf("%lld Philosopher %d %s\n", get_time() - philo->data->start_time,
			philo->id, status);
	pthread_mutex_unlock(&philo->data->print_lock);
}

void take_forks(t_philosopher *philo)
{
    if (philo->left_fork < philo->right_fork)
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

void	put_forks(t_philosopher *philo)
{
	pthread_mutex_unlock(philo->right_fork);
	pthread_mutex_unlock(philo->left_fork);
}

int check_death_and_meals(t_philosopher *philo)
{
    pthread_mutex_lock(&philo->data->dead_lock);
    if (philo->data->dead)
    {
        pthread_mutex_unlock(&philo->data->dead_lock);
        return 1;
    }
    if ((get_time() - philo->last_meal) > philo->data->time_to_die)
    {
        print_status(philo, "has died");
        philo->data->dead = 1;
        pthread_mutex_unlock(&philo->data->dead_lock);
        return 1;
    }
    if (philo->data->must_eat_count != -1 && philo->meals_eaten >= philo->data->must_eat_count)
    {
        pthread_mutex_lock(&philo->data->meal_lock);
        philo->data->finished_eating++;
        if (philo->data->finished_eating == philo->data->num_philos)
            philo->data->dead = 1;
        pthread_mutex_unlock(&philo->data->meal_lock);
        pthread_mutex_unlock(&philo->data->dead_lock);
        return 1;
    }
    pthread_mutex_unlock(&philo->data->dead_lock);
    return 0;
}

int	handle_single_philosopher(t_philosopher *philo)
{
	if (philo->data->num_philos == 1)
	{
		print_status(philo, "took right fork");
		usleep(philo->data->time_to_die * 1000);
		print_status(philo, "is died");
		return 1;
	}
    return 0;
}

void init_data(t_data *data, int argc, char **argv)
{
    data->num_philos = atoi(argv[1]);
    data->time_to_die = atoi(argv[2]);
    data->time_to_eat = atoi(argv[3]);
    data->time_to_sleep = atoi(argv[4]);
    data->must_eat_count = (argc == 6) ? atoi(argv[5]) : -1;
    data->dead = 0;
    data->finished_eating = 0;
    pthread_mutex_init(&data->dead_lock, NULL);
    pthread_mutex_init(&data->print_lock, NULL);
    data->forks = malloc(sizeof(pthread_mutex_t) * data->num_philos);
    if (!data->forks)
    {
        printf("Error: Failed to allocate memory for forks\n");
        exit(1);
    }
}

void eat(t_philosopher *philo)
{
    pthread_mutex_lock(philo->left_fork);
    print_status(philo, "took left fork");
    pthread_mutex_lock(philo->right_fork);
    print_status(philo, "took right fork");

    print_status(philo, "is eating");

    pthread_mutex_lock(&philo->data->dead_lock);
    philo->last_meal = get_time();
    philo->meals_eaten++;
    pthread_mutex_unlock(&philo->data->dead_lock);

    usleep(philo->data->time_to_eat * 1000);

    pthread_mutex_unlock(philo->right_fork);
    pthread_mutex_unlock(philo->left_fork);
}

void	init_forks(t_data *data)
{
	int	i;

	i = 0;
	while (i < data->num_philos)
	{
		pthread_mutex_init(&data->forks[i], NULL);
		i++;
	}
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
    pthread_mutex_destroy(&data->print_lock);
    free(data->forks);
}