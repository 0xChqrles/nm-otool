/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handle_sections.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: clanier <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/12/17 21:04:15 by clanier           #+#    #+#             */
/*   Updated: 2017/12/17 21:08:54 by clanier          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "otool.h"

int	get_section(t_file file,
struct segment_command *sg, t_txt_section *txt_sect)
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
		if (!ft_strcmp(s->sectname, file.opts & OPT_D ? SECT_DATA : SECT_TEXT))
		{
			txt_sect->offset = s->offset;
			txt_sect->addr = s->addr;
			txt_sect->size = s->size;
			txt_sect->find = true;
			if (check_size(&file, s->offset + s->size, F_BEGIN) < 0)
				return (-1);
		}
		if (check_size(&file, sizeof(struct section), F_OFFSET) < 0)
			return (-1);
		s = (struct section*)((size_t)s + sizeof(struct section));
	}
	return (0);
}

int	get_section_64(t_file file,
struct segment_command_64 *sg, t_txt_section *txt_sect)
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
		if (!ft_strcmp(s->sectname, file.opts & OPT_D ? SECT_DATA : SECT_TEXT))
		{
			txt_sect->offset = s->offset;
			txt_sect->addr = s->addr;
			txt_sect->size = s->size;
			txt_sect->find = true;
			if (check_size(&file, s->offset + s->size, F_BEGIN) < 0)
				return (-1);
		}
		if (check_size(&file, sizeof(struct section_64), F_OFFSET) < 0)
			return (-1);
		s = (struct section_64*)((size_t)s + sizeof(struct section_64));
	}
	return (0);
}
