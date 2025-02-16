#include "philo.h"

void	*philosopher_routine(void *arg)
{
	t_philosopher	*philo;
	//t_data *data = 0;

	philo = (t_philosopher *)arg;
	while (1)
	{
		
			
		take_forks(philo);
		//usleep(philo->data->time_to_eat * 1000);
		print_status(philo, "is eating");
		pthread_mutex_lock(&philo->data->dead_lock);
		philo->last_meal = get_time();
		philo->meals_eaten++;
		pthread_mutex_unlock(&philo->data->dead_lock);
		precise_sleep(time_of_philo(), philo->data->time_to_eat, philo);
		//usleep(philo->data->time_to_eat * 1000);
		put_forks(philo);
		if (philo->data->must_eat_count != -1 && philo->meals_eaten > philo->data->must_eat_count)
		{
			pthread_mutex_lock(&philo->data->dead_lock);
			philo->data->num_philos--;
			if (philo->data->num_philos == 0)
			{
				//usleep(philo->data->time_to_sleep * 1000);
				print_status(philo, "All philosophers have eaten enough times");
				philo->data->dead = 1;
			}
			pthread_mutex_unlock(&philo->data->dead_lock);
			exit (1);
		}
		//usleep(philo->data->time_to_sleep * 1000);
		print_status(philo, "is sleeping");
		 precise_sleep(time_of_philo(), philo->data->time_to_sleep, philo);
		//usleep(philo->data->time_to_sleep * 1000);
		print_status(philo, "is thinking");


		// if (check_death_and_meals(philo))
		// 	exit (1);
		pthread_mutex_lock(&philo->data->dead_lock);
		if (philo->data->dead || (get_time() - philo->last_meal) > philo->data->time_to_die)
		{
			//if (!philo->data->dead)
			//{
				//usleep(philo->data->time_to_sleep * 1000);
				print_status(philo, "has died");
				philo->data->dead = 1;
			//}
			pthread_mutex_unlock(&philo->data->dead_lock);
			break;
		}
		pthread_mutex_unlock(&philo->data->dead_lock);
	}
	return (NULL);
}

// int monitor_philos(t_data *data, t_philosopher *philos)
// {
// 	//t_philosopher *philo;
//     int i;
// 	int all_ate;

//     while (1)
//     {
// 		all_ate = 0;
//         i = 0;
//         while (i < data->num_philos)
//         {
//             pthread_mutex_lock(&data->dead_lock);
//             if ( ( get_time() - philos[i].last_meal) > data->time_to_die)
//             {
//                 print_status(&philos[i], "has died_mon");
//                 data->dead = 1;
//                 pthread_mutex_unlock(&data->dead_lock);
//                 return (0);
//             }
// 			// if ((philos->data->must_eat_count != -1 && philos->meals_eaten > philos->data->must_eat_count))
// 			// {
// 			// 	pthread_mutex_unlock(&philos->data->dead_lock);
// 			// 	pthread_exit(NULL);
// 			// }
// 			if (data->must_eat_count != -1 && philos[i].meals_eaten < data->must_eat_count)
// 				all_ate = 0;
//             pthread_mutex_unlock(&data->dead_lock);
// 			//check_death_and_meals(&philos[i]);
//             i++;
//         }
// 		if (data->must_eat_count != -1 && all_ate)
// 		{
// 			print_status(&philos[0], "All philosophers have eaten enough times");
// 			return (0);
// 		}
//          usleep(1000);
//     }
// 	return 0;
// }

int start_simulation(t_data *data, t_philosopher *philos)
{
	int i;

	data->start_time = get_time();
	i = 0;
	while (i < data->num_philos)
	{
		
		handle_single_philosopher(philos);
		
		if (pthread_create(&philos[i].thread, NULL, philosopher_routine, &philos[i]))
			return 0;
	
		
		pthread_mutex_lock(&data->dead_lock);
		philos[i].last_meal = get_time();
		pthread_mutex_unlock(&data->dead_lock);

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
            if ((get_time() - philos[i].last_meal) > data->time_to_die)
            {
                print_status(&philos[i], "has died");
                data->dead = 1;
                pthread_mutex_unlock(&data->dead_lock);
                return 0;
            }
            pthread_mutex_unlock(&data->dead_lock);
            i++;
        }
        usleep(100);
    }
	
	return (1);
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
	if (start_simulation(&data, philos))
	{
		printf("Simulation completed\n");
	}
	cleanup(&data, philos);
	return (0);
}