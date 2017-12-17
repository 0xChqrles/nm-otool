/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   fat.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: clanier <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/12/17 20:33:27 by clanier           #+#    #+#             */
/*   Updated: 2017/12/17 20:33:56 by clanier          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "nm.h"

void	print_binary_arch(char *name, cpu_type_t cpu)
{
	ft_printf("\n%s (for architecture", name);
	if (cpu == CPU_TYPE_X86)
		ft_printf(" i386");
	else if (cpu == CPU_TYPE_POWERPC)
		ft_printf(" ppc");
	ft_printf("):\n");
}

int		handle_fat_64(t_file file, uint32_t nfat_arch)
{
	struct fat_arch_64	*fat;
	int					i;

	if (nfat_arch < 1
	|| check_size(&file, sizeof(struct fat_arch_64) * nfat_arch, F_OFFSET) < 0)
		return (-1);
	i = nfat_arch;
	fat = (struct fat_arch_64*)((size_t)file.ptr + sizeof(struct fat_header));
	while (i && sw32(fat->cputype, file.arch) != CPU_TYPE_X86_64 && i--)
		fat = (struct fat_arch_64*)((size_t)fat + sizeof(struct fat_arch_64));
	fat = (struct fat_arch_64*)((size_t)file.ptr + sizeof(struct fat_header));
	while (nfat_arch--)
	{
		if (!i)
			print_binary_arch(file.name, sw32(fat->cputype, file.arch));
		file.offset = sw64(fat->offset, file.arch);
		if ((!i && get_arch(file, false) < 0)
		|| ((uint32_t)i == (nfat_arch + 1) && get_arch(file, true) < 0))
			return (-1);
		fat = (struct fat_arch_64*)((size_t)fat + sizeof(struct fat_arch_64));
	}
	return (0);
}

int		handle_fat(t_file file, uint32_t nfat_arch)
{
	struct fat_arch	*fat;
	int				i;

	if (nfat_arch < 1
	|| check_size(&file, sizeof(struct fat_arch) * nfat_arch, F_OFFSET) < 0)
		return (-1);
	i = nfat_arch;
	fat = (struct fat_arch*)((size_t)file.ptr + sizeof(struct fat_header));
	while (i && sw32(fat->cputype, file.arch) != CPU_TYPE_X86_64 && i--)
		fat = (struct fat_arch*)((size_t)fat + sizeof(struct fat_arch));
	fat = (struct fat_arch*)((size_t)file.ptr + sizeof(struct fat_header));
	while (nfat_arch--)
	{
		if (!i)
			print_binary_arch(file.name, sw32(fat->cputype, file.arch));
		file.offset = sw32(fat->offset, file.arch);
		if ((!i && get_arch(file, false) < 0)
		|| ((uint32_t)i == (nfat_arch + 1) && get_arch(file, true) < 0))
			return (-1);
		fat = (struct fat_arch*)((size_t)fat + sizeof(struct fat_arch));
	}
	return (0);
}

int		handle_fat_header(t_file file)
{
	struct fat_header	*fh;

	if (check_size(&file, sizeof(struct fat_header), F_OFFSET) < 0)
		return (-1);
	fh = (struct fat_header*)file.ptr;
	if (file.arch & ARCH_32)
		return (handle_fat(file, sw32(fh->nfat_arch, file.arch)));
	else if (file.arch & ARCH_64)
		return (handle_fat_64(file, sw32(fh->nfat_arch, file.arch)));
	return (-1);
}
