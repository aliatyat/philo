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

void	take_forks(t_philosopher *philo)
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

void	put_forks(t_philosopher *philo)
{
	pthread_mutex_unlock(philo->right_fork);
	pthread_mutex_unlock(philo->left_fork);
}

int	check_death_and_meals(t_philosopher *philo)
{
	pthread_mutex_lock(&philo->data->dead_lock);
	if (philo->data->dead || (philo->data->must_eat_count != -1
			&& philo->meals_eaten >= philo->data->must_eat_count))
	{
		pthread_mutex_unlock(&philo->data->dead_lock);
		pthread_exit(NULL);
	}
	pthread_mutex_unlock(&philo->data->dead_lock);

	pthread_mutex_lock(&philo->data->dead_lock);
	//pthread_mutex_lock(&philo->meal_mutex);
		if (philo->data->dead || (get_time() - philo->last_meal) > philo->data->time_to_die)
		{
			if (!philo->data->dead)
			{
				usleep(philo->data->time_to_sleep * 1000);
				print_status(philo, "has died");
				philo->data->dead = 1;
			}
			//pthread_mutex_unlock(&philo->meal_mutex);
			pthread_mutex_unlock(&philo->data->dead_lock);
			return 1;
		}
	
	
	return (0);
}

void	handle_single_philosopher(t_philosopher *philo)
{
	if (philo->data->num_philos == 1)
	{
		//usleep(philo->data->time_to_sleep * 1000);
		print_status(philo, "took right fork");
		usleep(philo->data->time_to_die * 1000);
		print_status(philo, "is died");
		pthread_exit(NULL);
	}
}

long long	time_of_philo(void)
{
	struct timeval	time;
	long long		current_time;

	gettimeofday(&time, NULL);
	current_time = (time.tv_sec * 1000) + (time.tv_usec / 1000);
	return (current_time);
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
void	init_data(t_data *data, t_philosopher *philos, int argc, char **argv)
{
	int i;
	data->num_philos = atoi(argv[1]);
	data->time_to_die = atoi(argv[2]);
	data->time_to_eat = atoi(argv[3]);
	data->time_to_sleep = atoi(argv[4]);
	if (argc == 6)
		data->must_eat_count = atoi(argv[5]);
	else
		data->must_eat_count = -1;
	data->dead = 0;
	pthread_mutex_init(&data->dead_lock, NULL);
	pthread_mutex_init(&data->print_lock, NULL);
	i = 0;
	while (i < data->num_philos)
	{
	    pthread_mutex_init(&philos[i].meal_mutex, NULL);
		pthread_mutex_init(&philos[i].all_done, NULL);
	    i++;
	}
	data->forks = malloc(sizeof(pthread_mutex_t) * data->num_philos);
	if (!data->forks)
	{
		free (data->forks);
		return ;
	}
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

void	cleanup(t_data *data, t_philosopher *philos)
{
	int i;

	i = 0;
	(void) philos;
	while (i < data->num_philos)
	{
		pthread_join(philos[i].thread, NULL);
		pthread_mutex_destroy(&data->forks[i]);
		i++;
	}
	pthread_mutex_destroy(&data->dead_lock);
	pthread_mutex_destroy(&data->print_lock);
	free(data->forks);
}