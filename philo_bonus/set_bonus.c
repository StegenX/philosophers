/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   set_bonus.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aagharbi <aagharbi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/04 13:57:21 by aagharbi          #+#    #+#             */
/*   Updated: 2025/04/07 08:45:11 by aagharbi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

long long	get_last_meal(t_philo *philo)
{
	long long	last_meal;

	last_meal = philo->last_meal;
	return (last_meal);
}

void	set_last_meal(t_philo *philo)
{
	sem_wait(philo->data->meal);
	philo->last_meal = get_time();
	sem_post(philo->data->meal);
	philo->meals_eaten++;
}

void	norm(t_philo *philo)
{
	print_action(philo, "is eating");
	set_last_meal(philo);
	usleep(philo->data->time_to_eat * 1000);
}

void	norm2(t_philo *philo)
{
	print_action(philo, "is thinking");
	usleep(1000);
	take_forks(philo);
}
