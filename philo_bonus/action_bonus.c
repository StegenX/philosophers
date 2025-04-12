/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   action_bonus.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aagharbi <aagharbi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/05 17:21:09 by aagharbi          #+#    #+#             */
/*   Updated: 2025/04/07 09:22:55 by aagharbi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

void	take_forks(t_philo *philo)
{
	sem_wait(philo->data->forks);
	print_action(philo, "has taken a fork");
	sem_wait(philo->data->forks);
	print_action(philo, "has taken a fork");
}

void	print_action(t_philo *philo, char *action)
{
	long long	start_time;

	sem_wait(philo->data->last_print);
	start_time = philo->data->start_time;
	printf("%lld %d %s\n", get_time() - start_time, philo->id, action);
	sem_post(philo->data->last_print);
}

void	drop_forks(t_philo *philo)
{
	sem_post(philo->data->forks);
	sem_post(philo->data->forks);
}
