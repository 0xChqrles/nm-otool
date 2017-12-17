/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handle_types.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: clanier <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/12/17 20:24:48 by clanier           #+#    #+#             */
/*   Updated: 2017/12/17 20:32:22 by clanier          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "nm.h"

char	get_secttype(t_sect **sect, uint16_t n_sect)
{
	t_sect	*tmp;

	tmp = *sect;
	while (tmp)
	{
		if (tmp->n == n_sect)
			return (tmp->type);
		tmp = tmp->next;
	}
	return ('S');
}

char	get_type(uint64_t n_value, uint8_t n_sect, uint8_t n_type, t_sect *sect)
{
	char		type;
	uint16_t	n_type_mask;

	type = '?';
	n_type_mask = n_type & N_TYPE;
	if (n_type_mask == N_UNDF)
		type = n_value ? 'C' : 'U';
	else if (n_type_mask == N_PBUD)
		type = 'U';
	else if (n_type_mask == N_ABS)
		type = 'A';
	else if (n_type_mask == N_INDR)
		type = 'I';
	else if (n_type_mask == N_SECT)
		type = get_secttype(&sect, n_sect);
	if (n_type_mask & N_STAB)
		type = 'Z';
	if (!(n_type & N_EXT) && type != '?')
		type += 32;
	return (type);
}
