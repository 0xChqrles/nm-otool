/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   symbols.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: clanier <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/12/17 20:11:01 by clanier           #+#    #+#             */
/*   Updated: 2017/12/17 20:14:18 by clanier          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "nm.h"

t_symbol	*new_symbol(uint64_t value, char type, char *name)
{
	t_symbol	*new;

	if (!(new = malloc(sizeof(t_symbol))))
		return (NULL);
	new->next = NULL;
	new->value = value;
	new->name = name;
	new->type = type;
	return (new);
}

int			add_symbol(t_symbol **syms, uint64_t value, char type, char *name)
{
	t_symbol	*tmp;

	if (!(*syms))
	{
		if (!(*syms = new_symbol(value, type, name)))
			return (-1);
		return (0);
	}
	tmp = *syms;
	while (tmp->next)
		tmp = tmp->next;
	if (!(tmp->next = new_symbol(value, type, name)))
		return (-1);
	return (0);
}

void		free_symbol(t_symbol **syms)
{
	t_symbol	*tmp;
	t_symbol	*tmp2;

	tmp = *syms;
	while (tmp)
	{
		tmp2 = tmp;
		tmp = tmp->next;
		free(tmp);
	}
}
