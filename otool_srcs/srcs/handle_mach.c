/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handle_mach.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: clanier <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/12/17 21:07:24 by clanier           #+#    #+#             */
/*   Updated: 2017/12/17 21:08:03 by clanier          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "otool.h"

int	handle_mach(t_file file, int ncmds, struct load_command *lc)
{
	t_txt_section	txt_sect;

	txt_sect.find = false;
	while (ncmds--)
	{
		if (sw32(lc->cmd, file.arch) == LC_SEGMENT_64 && (file.arch & ARCH_32
		|| get_section_64(file, (struct segment_command_64*)lc, &txt_sect) < 0))
			return (-1);
		if (sw32(lc->cmd, file.arch) == LC_SEGMENT && (file.arch & ARCH_64
		|| get_section(file, (struct segment_command*)lc, &txt_sect) < 0))
			return (-1);
		if (check_size(&file, sw32(lc->cmdsize, file.arch), F_OFFSET) < 0
		|| !lc->cmdsize || (file.arch & ARCH_64 && lc->cmdsize % 8)
		|| (file.arch & ARCH_64 && lc->cmdsize % 4))
			return (-1);
		lc = (void*)((size_t)lc + sw32(lc->cmdsize, file.arch));
	}
	if (txt_sect.find)
		dump_section(file, txt_sect.offset, txt_sect.size, txt_sect.addr);
	return (0);
}

int	handle_mach_header(t_file file, struct mach_header *mh)
{
	struct mach_header_64	*mh_64;

	if (check_size(&file, sizeof(struct load_command), F_OFFSET) < 0)
		return (-1);
	if (file.arch & ARCH_32)
	{
		if (check_size(&file, sizeof(struct mach_header_64), F_OFFSET) < 0)
			return (-1);
		return (handle_mach(file, sw32(mh->ncmds, file.arch),
		(struct load_command*)((size_t)file.ptr + sizeof(*mh))));
	}
	else if (file.arch & ARCH_64)
	{
		if (check_size(&file, sizeof(struct mach_header_64), F_OFFSET) < 0)
			return (-1);
		mh_64 = (struct mach_header_64*)file.ptr;
		return (handle_mach(file, sw32(mh_64->ncmds, file.arch),
		(struct load_command*)((size_t)file.ptr + sizeof(*mh_64))));
	}
	return (-1);
}
