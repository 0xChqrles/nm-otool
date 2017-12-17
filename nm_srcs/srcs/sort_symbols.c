/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sort_symbols.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: clanier <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/12/17 20:14:29 by clanier           #+#    #+#             */
/*   Updated: 2017/12/17 20:47:48 by clanier          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "nm.h"

void	swap_symbols(t_symbol **syms,
t_symbol **tmp, t_symbol **next, t_symbol **prev)
{
	if (*prev)
		(*prev)->next = (*tmp)->next;
	else
		*syms = (*tmp)->next;
	(*next) = (*tmp)->next->next;
	(*tmp)->next->next = *tmp;
	(*tmp)->next = *next;
	*tmp = (*tmp)->next;
}

void	reverse_symbols(t_symbol **syms)
{
	t_symbol	*prev;
	t_symbol	*tmp;
	t_symbol	*next;

	if (!*syms || !(*syms)->next)
		return ;
	tmp = *syms;
	next = tmp->next;
	prev = NULL;
	while (next)
	{
		tmp->next = prev;
		prev = tmp;
		tmp = next;
		next = next->next;
	}
	tmp->next = prev;
	*syms = tmp;
}

void	bubblesort_symbols(t_symbol **syms)
{
	t_symbol	*prev;
	t_symbol	*tmp;
	t_symbol	*next;
	bool		is_sort;
	char		cmp;

	is_sort = false;
	while (!is_sort && (is_sort = true))
	{
		tmp = *syms;
		prev = NULL;
		while (tmp && tmp->next)
		{
			cmp = ft_strcmp(tmp->name, tmp->next->name);
			if ((cmp > 0 || (!cmp && tmp->type == 'I')) && !(is_sort = false))
				swap_symbols(syms, &tmp, &next, &prev);
			prev = tmp;
			tmp = tmp ? tmp->next : NULL;
		}
	}
}
