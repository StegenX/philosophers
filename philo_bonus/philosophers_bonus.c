/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philosophers_bonus.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aagharbi <aagharbi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/05 17:20:56 by aagharbi          #+#    #+#             */
/*   Updated: 2025/04/12 13:48:56 by aagharbi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

void	kill_philos(t_data *data)
{
	int	i;

	i = 0;
	while (i < data->num_philos)
	{
		kill(data->philos[i].pid, SIGKILL);
		i++;
	}
}

void	*check_death(void *p)
{
	t_philo	*philo;
	long	last_meal;

	philo = (t_philo *)p;
	while (1)
	{
		sem_wait(philo->data->meal);
		last_meal = philo->last_meal;
		sem_post(philo->data->meal);
		if (get_time() - last_meal > philo->data->time_to_die)
		{
			sem_wait(philo->data->last_print);
			printf("%lld %d died\n", get_time() - philo->data->start_time,
				philo->id);
			sem_post(philo->data->death);
			exit(0);
		}
		usleep(500);
	}
}

void	philosopher_routine(t_philo *philo)
{
	if (pthread_create(&philo->data->thread_monitor, NULL, check_death,
			philo) != 0)
		exit((clean_up(philo->data), kill_philos(philo->data),
				printf("Thread faild\n"), 1));
	if (philo->id % 2 == 0)
		usleep(1000);
	while (1)
	{
		norm2(philo);
		if (philo->data->num_philos == 1)
		{
			drop_forks(philo);
			break ;
		}
		norm(philo);
		drop_forks(philo);
		if (philo->meals_eaten >= philo->data->num_meals
			&& philo->data->num_meals > 0)
			exit((pthread_join(philo->data->thread_monitor, NULL),
					sem_post(philo->data->lock), clean_up(philo->data), 0));
		print_action(philo, "is sleeping");
		usleep(philo->data->time_to_sleep * 1000);
	}
	exit((pthread_join(philo->data->thread_monitor, NULL),
			clean_up(philo->data), 0));
}

void	wait_process(t_data *data)
{
	int	i;

	i = 0;
	sem_wait(data->death);
	while (i++ < data->num_philos)
		sem_post(data->lock);
	pthread_join(data->monitor, NULL);
	kill_philos(data);
}

int	main(int argc, char **argv)
{
	t_data	data;
	int		i;

	i = 0;
	if (argc < 5 || argc > 6 || init_data(&data, argv, argc))
		return (printf("Error\n"), 1);
	init_philosophers(&data);
	data.start_time = get_time();
	while (i < data.num_philos)
	{
		data.philos[i].pid = fork();
		if (data.philos[i].pid == 0)
		{
			philosopher_routine(&data.philos[i]);
			exit(0);
		}
		i++;
	}
	if (pthread_create(&data.monitor, NULL, freez, &data) != 0)
		exit((clean_up(&data), printf("Thread faild\n"), 1));
	wait_process(&data);
	clean_up(&data);
	return (0);
}
