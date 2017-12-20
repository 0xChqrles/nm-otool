/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   arch.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: clanier <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/12/17 20:34:30 by clanier           #+#    #+#             */
/*   Updated: 2017/12/17 20:35:11 by clanier          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "nm.h"

uint8_t	is_valid_arch(uint32_t magic)
{
	return (magic == MH_MAGIC_64 || magic == MH_CIGAM_64
	|| magic == FAT_MAGIC_64 || magic == FAT_CIGAM_64
	|| magic == MH_MAGIC || magic == MH_CIGAM
	|| magic == FAT_MAGIC || magic == FAT_CIGAM ? ARCH_VAL : 0);
}

uint8_t	is_64_arch(uint32_t magic)
{
	return (magic == MH_MAGIC_64 || magic == MH_CIGAM_64
	|| magic == FAT_MAGIC_64 || magic == FAT_CIGAM_64 ? ARCH_64 : ARCH_32);
}

uint8_t	is_cigam_arch(uint32_t magic)
{
	return (magic == MH_CIGAM_64 || magic == MH_CIGAM
	|| magic == FAT_CIGAM || magic == FAT_CIGAM_64 ? CIGAM : MAGIC);
}

uint8_t	is_mach_arch(uint32_t magic)
{
	return (magic == MH_MAGIC_64 || magic == MH_MAGIC
	|| magic == MH_CIGAM || magic == MH_CIGAM_64 ? MACH_O : FAT);
}

int		get_arch(t_file file, bool print_name)
{
	struct mach_header	*mh;

	if (file.offset)
	{
		file.size -= file.offset;
		file.free_size = file.size;
		file.ptr = (char*)((size_t)file.ptr + file.offset);
	}
	if (check_size(&file, sizeof(struct mach_header), F_BEGIN) < 0)
		return (-1);
	mh = (struct mach_header*)file.ptr;
	file.arch = is_valid_arch(mh->magic) | is_mach_arch(mh->magic)
	| is_64_arch(mh->magic) | is_cigam_arch(mh->magic);
	if (print_name && (file.opts >> 8) > 1 && file.arch & MAGIC)
		file.print_name = true;
	if (file.arch & ARCH_VAL && file.arch & MACH_O)
		return (handle_mach_header(file, mh));
	else if (file.arch & ARCH_VAL && file.arch & FAT)
		return (handle_fat_header(file));
	else if (!ft_strncmp(file.ptr, ARMAG, SARMAG))
	{
		ft_printf("{red}[NM]{eoc} static library\n");
		return (0);
	}
	return (-2);
}
