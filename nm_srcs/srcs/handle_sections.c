/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handle_sections.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: clanier <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/12/17 20:27:30 by clanier           #+#    #+#             */
/*   Updated: 2017/12/17 20:28:44 by clanier          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "nm.h"

int	handle_sectname(char *sectname, t_sect **sect)
{
	if ((!ft_strcmp(sectname, SECT_TEXT)))
		return (add_sect(sect, 'T'));
	else if (!ft_strcmp(sectname, SECT_DATA))
		return (add_sect(sect, 'D'));
	else if (!ft_strcmp(sectname, SECT_BSS))
		return (add_sect(sect, 'B'));
	else
		return (add_sect(sect, 'S'));
}

int	get_section(t_file file, struct segment_command *sg, t_sect **sect)
{
	struct section	*s;
	uint32_t		i;

	i = 0;
	s = (struct section*)((size_t)sg + sizeof(struct segment_command));
	if (check_size(&file, (long int)sizeof(struct segment_command)
	- (long int)sw32(sg->cmdsize, file.arch)
	+ (long int)sizeof(struct section), F_OFFSET) < 0)
		return (-1);
	while (i++ < sw32(sg->nsects, file.arch))
	{
		if (handle_sectname(s->sectname, sect) < 0)
			return (-1);
		if (check_size(&file, sizeof(struct section), F_OFFSET) < 0)
			return (-1);
		s = (struct section*)((size_t)s + sizeof(struct section));
	}
	return (0);
}

int	get_section_64(t_file file, struct segment_command_64 *sg, t_sect **sect)
{
	struct section_64	*s;
	uint32_t			i;

	i = 0;
	s = (struct section_64*)((size_t)sg + sizeof(struct segment_command_64));
	if (check_size(&file, (long int)sizeof(struct segment_command_64)
	- (long int)sw32(sg->cmdsize, file.arch)
	+ (long int)sizeof(struct section_64), F_OFFSET) < 0)
		return (-1);
	while (i++ < sw32(sg->nsects, file.arch))
	{
		if (handle_sectname(s->sectname, sect) < 0)
			return (-1);
		if (check_size(&file, sizeof(struct section_64), F_OFFSET) < 0)
			return (-1);
		s = (struct section_64*)((size_t)s + sizeof(struct section_64));
	}
	return (0);
}
