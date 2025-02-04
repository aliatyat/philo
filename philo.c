#include "philo.h"

void	*philosopher_routine(void *arg)
{
	t_philosopher	*philo;

	philo = (t_philosopher *)arg;
	while (1)
	{
		check_death_and_meals(philo);
		handle_single_philosopher(philo);
		print_status(philo, "is thinking");
		take_forks(philo);
		print_status(philo, "is eating");
		pthread_mutex_lock(&philo->data->dead_lock);
		philo->last_meal = get_time();
		philo->meals_eaten++;
		pthread_mutex_unlock(&philo->data->dead_lock);
		usleep(philo->data->time_to_eat * 1000);
		put_forks(philo);
		print_status(philo, "is sleeping");
		usleep(philo->data->time_to_sleep * 1000);
	}
	return (NULL);
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

int	start_simulation(t_data *data, t_philosopher *philos)
{
	int	i;

	data->start_time = get_time();
	i = 0;
	while (i < data->num_philos)
	{
		if ((get_time() - philos[i].last_meal) > data->time_to_die)
		{
			print_status(&philos[i], "has died");
			data->dead = 1;
			pthread_mutex_unlock(&data->dead_lock);
			return (0);
		}
		if (pthread_create(&philos[i].thread, NULL, philosopher_routine,
				&philos[i]))
			return (0);
		pthread_mutex_lock(&philos->data->dead_lock);
		philos[i].last_meal = get_time();
		pthread_mutex_unlock(&philos->data->dead_lock);
		i++;
	}
	return (0);
}

int	init_philosophers(t_data *data, t_philosopher *philos)
{
	int	i;

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
	return (0);
}

int	validate_args(int argc, char **argv)
{
	if (argc != 5 && argc != 6)
		return (0);
	if (atoi(argv[1]) <= 0 || atoi(argv[2]) <= 0 || atoi(argv[3]) <= 0
		|| atoi(argv[4]) <= 0)
		return (0);
	return (1);
}

int	main(int argc, char **argv)
{
	t_philosopher philos[250];
	t_data data;

	if (!validate_args(argc, argv))
	{
		printf("Error Number\n");
		return (1);
	}
	init_data(&data, argc, argv);
	init_forks(&data);
	init_philosophers(&data, philos);
	start_simulation(&data, philos);
	cleanup(&data, philos);
	return (0);
}