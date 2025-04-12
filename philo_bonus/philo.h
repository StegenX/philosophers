/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aagharbi <aagharbi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/05 17:21:17 by aagharbi          #+#    #+#             */
/*   Updated: 2025/04/07 08:45:26 by aagharbi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PHILO_H
# define PHILO_H

# include <fcntl.h>
# include <pthread.h>
# include <semaphore.h>
# include <signal.h>
# include <stdio.h>
# include <stdlib.h>
# include <sys/time.h>
# include <sys/wait.h>
# include <unistd.h>

typedef struct s_philo
{
	int				id;
	int				meals_eaten;
	long long		last_meal;
	int				pid;
	struct s_data	*data;
}					t_philo;

typedef struct s_data
{
	sem_t			*forks;
	sem_t			*death;
	sem_t			*last_print;
	sem_t			*lock;
	sem_t			*meal;
	sem_t			*print;
	int				negative_value;
	long long		num_philos;
	long long		time_to_die;
	long long		time_to_eat;
	long long		time_to_sleep;
	long long		num_meals;
	long long		start_time;
	pthread_t		thread_monitor;
	pthread_t		monitor;
	t_philo			*philos;

}					t_data;

long long			get_time(void);
void				print_action(t_philo *philo, char *action);
int					init_data(t_data *data, char **argv, int ac);
void				init_philosophers(t_data *data);
void				philosopher_routine(t_philo *philo);
void				clean_up(t_data *data);
void				drop_forks(t_philo *philo);
void				init_norm(t_data *data);
void				take_forks(t_philo *philo);
long long			get_last_meal(t_philo *philo);
void				set_last_meal(t_philo *philo);
void				wait_process(t_data *data);
void				*freez(void *arg);
void				norm(t_philo *philo);
void				norm2(t_philo *philo);
long long			ft_atoi(const char *nptr, t_data *my_data);

#endif
