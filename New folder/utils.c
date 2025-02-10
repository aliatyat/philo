#include "philo.h"

long long get_time(void)
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return ((tv.tv_sec * 1000LL) + (tv.tv_usec / 1000));
}

void print_status(t_philosopher *philo, char *status)
{
    pthread_mutex_lock(&philo->data->print_lock);
    if (!philo->data->dead)
        printf("%lld Philosopher %d %s\n", get_time() - philo->data->start_time, philo->id, status);
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

int check_death_and_meals(t_philosopher *philo)
{
    pthread_mutex_lock(&philo->data->dead_lock);
    if (philo->data->dead)
    {
        pthread_mutex_unlock(&philo->data->dead_lock);
        return 1;
    }
    // if ((get_time() - philo->last_meal) > philo->data->time_to_die)
    // {
    //     printf("hereee");
    //      print_status(philo, "has died");
    //     philo->data->dead = 1;
    //     pthread_mutex_unlock(&philo->data->dead_lock);
    //     return 1;
    // }
    if (philo->data->must_eat_count != -1 && philo->meals_eaten > philo->data->must_eat_count)
    {
        pthread_mutex_lock(&philo->data->meal_lock);
        philo->data->finished_eating++;
        if (philo->data->finished_eating == philo->data->num_philos)
            philo->data->dead = 1;
        pthread_mutex_unlock(&philo->data->meal_lock);
        pthread_mutex_unlock(&philo->data->dead_lock);
        return(1);
    }
    pthread_mutex_unlock(&philo->data->dead_lock);
    return 0;
}

void *philosopher_routine(void *arg)
{
    t_philosopher *philo = (t_philosopher *)arg;
    while (1)
    {
        if(handle_single_philosopher(philo))
            return(0);
        pthread_mutex_lock(&philo->data->dead_lock);
        if (philo->data->dead)
        {
            pthread_mutex_unlock(&philo->data->dead_lock);
            break;
        }
        pthread_mutex_unlock(&philo->data->dead_lock);

        print_status(philo, "is thinking");
        take_forks(philo);
        print_status(philo, "is eating");

        pthread_mutex_lock(&philo->data->dead_lock);
        philo->last_meal = get_time();
        philo->meals_eaten++;
        pthread_mutex_unlock(&philo->data->dead_lock);

        // usleep(philo->data->time_to_eat * 1000);
        precise_sleep(time_of_philo(), philo->data->time_to_eat, philo);

        put_forks(philo);

        print_status(philo, "is sleeping");
        // usleep(philo->data->time_to_sleep * 1000);
        precise_sleep(time_of_philo(), philo->data->time_to_sleep, philo);

        if (check_death_and_meals(philo))
            exit (1);
    }
    return NULL;
}


long long	time_of_philo(void)
{
	struct timeval	time;
	long long		current_time;

	gettimeofday(&time, NULL);
	current_time = (time.tv_sec * 1000) + (time.tv_usec / 1000);
	return (current_time);
}

int	handle_single_philosopher(t_philosopher *philo)
{
	if (philo->data->num_philos == 1)
	{
		print_status(philo, "took right fork");
        precise_sleep(time_of_philo(), philo->data->time_to_die, philo);
		//usleep(philo->data->time_to_die * 1000);
		print_status(philo, "is died");
		return(1);
	}
	return 0;
}

void	precise_sleep(long long start_time, long long duration_ms,
						t_philosopher *philo)
{
	long long	end_time;
    
    (void)philo;
	end_time = start_time + duration_ms;
	while (time_of_philo() < end_time)
	{
		// pthread_mutex_lock(&philo->simulation->data_lock);
        pthread_mutex_lock(&philo->data->dead_lock);
		if (philo->data->dead == 1)
		{
			// pthread_mutex_unlock(&philo->simulation->data_lock);
            pthread_mutex_unlock(&philo->data->dead_lock);
			break ;
		}
		// pthread_mutex_unlock(&philo->simulation->data_lock);
        pthread_mutex_unlock(&philo->data->dead_lock);
		usleep(1);
	}
}

int start_simulation(t_data *data)
{
    int i = 0;
    while (i < data->num_philos)
    {
        if (pthread_create(&data->philosophers[i].thread, NULL, philosopher_routine, &data->philosophers[i]))
            return 0;
        i++;
    }

    while (1)
    {
        i = 0;
        while (i < data->num_philos)
        {
            pthread_mutex_lock(&data->dead_lock);
            if (data->dead)
            {
                pthread_mutex_unlock(&data->dead_lock);
                return 0;
            }
            if ((get_time() - data->philosophers[i].last_meal) > data->time_to_die)
            {
                print_status(&data->philosophers[i], "has died");
                data->dead = 1;
                pthread_mutex_unlock(&data->dead_lock);
                return 0;
            }
            pthread_mutex_unlock(&data->dead_lock);
            i++;
        }
        usleep(1000);
    }
    return 0;
}

void cleanup(t_data *data)
{
    int i = 0;
    while (i < data->num_philos)
    {
        pthread_join(data->philosophers[i].thread, NULL);
        pthread_mutex_destroy(&data->forks[i]);
        i++;
    }
    pthread_mutex_destroy(&data->dead_lock);
    pthread_mutex_destroy(&data->print_lock);
    pthread_mutex_destroy(&data->meal_lock);
    free(data->forks);
}