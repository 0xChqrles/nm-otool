/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   check_size.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: clanier <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/12/17 20:31:39 by clanier           #+#    #+#             */
/*   Updated: 2017/12/17 20:46:47 by clanier          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "nm.h"

int	check_size(t_file *file, long int size, uint8_t flag)
{
	if (flag & F_BEGIN && file->size - size < 0)
		return (-1);
	if (flag & F_OFFSET && (file->free_size -= size) < 0)
		return (-1);
	return (0);
}

int		check_strtab(t_file file, char *strtab, int64_t symoff, uint32_t stroff)
{
	int		i;

	i = 0;
	if (check_size(&file, symoff, F_BEGIN) < 0
	|| check_size(&file, stroff, F_BEGIN) < 0)
		return (-1);
	while (strtab[i++])
		if (check_size(&file, stroff + i, F_BEGIN) < 0)
			return (-1);
	return (0);
}

int	check_symtab(t_file file, struct symtab_command *sym,
size_t sym_offset, t_sect *sect)
{
	if ((file.free_size = file.size - sym_offset + sizeof(struct load_command)
	- sizeof(struct symtab_command)) <= 0
	|| (check_size(&file, sw32(sym->symoff, file.arch), F_BEGIN) < 0)
	|| (check_size(&file, sw32(sym->stroff, file.arch), F_BEGIN) < 0)
	|| (file.arch & ARCH_64 && get_symbols_64(file, sym, sect) < 0)
	|| (file.arch & ARCH_32 && get_symbols(file, sym, sect) < 0))
		return (-1);
	return (0);
}
