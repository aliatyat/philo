#ifndef PHILO_H
# define PHILO_H

# include <pthread.h>
# include <stdio.h>
# include <stdlib.h>
# include <sys/time.h>
# include <unistd.h>

// Define the philosopher structure
typedef struct s_philosopher
{
	int				id;
	int				meals_eaten;
	
	long long		last_meal;
	
	pthread_mutex_t	*left_fork;
	pthread_mutex_t	*right_fork;
	struct s_data	*data;
	
	pthread_t		thread;
	pthread_mutex_t meal_mutex;
}					t_philosopher;

// Define the data structure for shared information
typedef struct s_data
{
	pthread_mutex_t				meals_eaten2;
	int				num_philos;
	int				time_to_die;
	int				time_to_eat;
	int				time_to_sleep;
	int				must_eat_count;
	int				dead;
	long long		start_time;
	pthread_mutex_t	*forks;
	pthread_mutex_t	dead_lock;
	pthread_mutex_t	print_lock;
}					t_data;

// Function prototypes
long long			get_time(void);
void				print_status(t_philosopher *philo, char *status);
void				take_forks(t_philosopher *philo);
void				put_forks(t_philosopher *philo);
void				*philosopher_routine(void *arg);
int					start_simulation(t_data *data, t_philosopher *philos);
int					init_philosophers(t_data *data, t_philosopher *philos);
long long			get_time(void);
void				print_status(t_philosopher *philo, char *status);
void				take_forks(t_philosopher *philo);
void				put_forks(t_philosopher *philo);
int				check_death_and_meals(t_philosopher *philo);
void				handle_single_philosopher(t_philosopher *philo);
void	init_data(t_data *data, t_philosopher *philos, int argc, char **argv);
void				init_forks(t_data *data);
void				cleanup(t_data *data, t_philosopher *philos);
//int monitor_philos(t_data *data, t_philosopher *philos);
//void	smart_sleep(long long time, t_data *data);
void	precise_sleep(long long start_time, long long duration_ms,
						t_philosopher *philo);
						long long	time_of_philo(void);

#endif // PHILO