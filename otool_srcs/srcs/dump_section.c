/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dump_section.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: clanier <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/12/17 21:06:35 by clanier           #+#    #+#             */
/*   Updated: 2017/12/17 21:25:09 by clanier          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "otool.h"

void	dump_section(t_file file, uint32_t offset, uint64_t size, uint64_t addr)
{
	uint64_t	i;

	i = 0;
	file.ptr = (char*)((size_t)file.ptr + offset);
	ft_printf("Contents of %s section", file.opts & OPT_D
	? "(__DATA,__data)" : "(__TEXT,__text)");
	while (i < size)
	{
		if (!(i % 16) && i < size)
			ft_printf(file.arch & ARCH_64
			? "\n%016lx\t" : "\n%08lx\t", addr + i);
		ft_printf("%02hhx ", (uint8_t)(*(file.ptr)));
		file.ptr++;
		i++;
	}
	ft_printf("\n");
}
