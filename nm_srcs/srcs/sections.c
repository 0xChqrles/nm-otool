/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sections.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: clanier <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/12/17 20:19:20 by clanier           #+#    #+#             */
/*   Updated: 2017/12/17 20:22:07 by clanier          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "nm.h"

void	*new_sect(char type, int n)
{
	t_sect	*new;

	if (!(new = malloc(sizeof(t_sect))))
		return (NULL);
	new->type = type;
	new->n = n;
	new->next = NULL;
	return (new);
}

int		add_sect(t_sect **sect, char type)
{
	t_sect	*tmp;
	int		n;

	n = 1;
	if (!(*sect))
	{
		if (!(*sect = new_sect(type, n)))
			return (-1);
		return (0);
	}
	tmp = *sect;
	while (++n && tmp->next)
		tmp = tmp->next;
	if (!(tmp->next = new_sect(type, n)))
		return (-1);
	return (0);
}

void	free_sect(t_sect **sect)
{
	t_sect	*tmp;
	t_sect	*tmp2;

	tmp = *sect;
	while (tmp)
	{
		tmp2 = tmp;
		tmp = tmp->next;
		free(tmp);
	}
}
