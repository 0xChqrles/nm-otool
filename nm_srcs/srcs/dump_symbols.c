/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handle_symbols.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: clanier <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/12/17 20:17:08 by clanier           #+#    #+#             */
/*   Updated: 2017/12/17 20:49:17 by clanier          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "nm.h"

void	print_symbols(t_file file, t_symbol **syms)
{
	t_symbol	*tmp;

	tmp = *syms;
	while (tmp)
	{
		if (((tmp->type == 'U' || tmp->type == 'u') && file.opts & OPT_MU)
		|| (tmp->type != 'U' && tmp->type != 'u' && file.opts & OPT_U))
		{
			tmp = tmp->next;
			continue ;
		}
		if (!(file.opts & OPT_U) && !(file.opts & OPT_J))
		{
			if (tmp->type == 'U' || tmp->type == 'u')
				ft_printf(file.arch & ARCH_64 ? "%17c" : "%9c", ' ');
			else
				ft_printf(file.arch & ARCH_64
				? "%016llx " : "%08llx ", tmp->value);
			ft_printf("%1c ", tmp->type);
		}
		ft_printf("%s\n", tmp->name);
		tmp = tmp->next;
	}
}

int		dump_symbols(t_file file, t_symbol **syms)
{
	if (!(*syms))
		return (-1);
	if (!(file.opts & OPT_P))
	{
		bubblesort_symbols(syms);
		if ((file.opts & OPT_R))
			reverse_symbols(syms);
	}
	print_symbols(file, syms);
	free_symbol(syms);
	return (0);
}
