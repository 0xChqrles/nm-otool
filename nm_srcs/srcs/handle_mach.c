/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handle_mach.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: clanier <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/12/17 20:29:39 by clanier           #+#    #+#             */
/*   Updated: 2017/12/17 20:50:50 by clanier          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "nm.h"

uint32_t	get_nvalue_32(struct nlist sym, uint8_t arch)
{
	if ((sym.n_type & N_TYPE) == N_SECT
	&& sw16(sym.n_desc, arch) == N_ARM_THUMB_DEF)
		return (sw32(sw32(sym.n_value, arch) | 1, arch));
	return (sym.n_value);
}

int			get_symbols(t_file file, struct symtab_command *sym, t_sect *sect)
{
	int				i;
	char			*strtab;
	struct nlist	*symtab;
	t_symbol		*syms;
	char			type;

	i = 0;
	syms = NULL;
	symtab = (struct nlist*)((size_t)file.ptr + sw32(sym->symoff, file.arch));
	strtab = (char*)((size_t)file.ptr + sw32(sym->stroff, file.arch));
	while ((uint32_t)i < sw32(sym->nsyms, file.arch))
	{
		if (symtab[i].n_type & N_STAB && ++i)
			continue;
		symtab[i].n_value = get_nvalue_32(symtab[i], file.arch);
		type = get_type(sw32(symtab[i].n_value, file.arch),
		symtab[i].n_sect, symtab[i].n_type, sect);
		if (add_symbol(&syms, sw32(symtab[i].n_value, file.arch),
		type, strtab + sw32(symtab[i].n_un.n_strx, file.arch)) < 0
		|| check_size(&file, sw32(sym->symoff, file.arch) + (i + 1)
		* sizeof(struct nlist), F_BEGIN) < 0)
			return (-1);
		i++;
	}
	return (dump_symbols(file, &syms));
}

int			get_symbols_64(t_file file,
struct symtab_command *sym, t_sect *sect)
{
	int				i;
	char			*strtab;
	struct nlist_64	*symtab;
	t_symbol		*syms;
	char			type;

	i = 0;
	syms = NULL;
	symtab = (struct nlist_64*)((size_t)file.ptr
	+ sw32(sym->symoff, file.arch));
	strtab = (char*)((size_t)file.ptr + sw32(sym->stroff, file.arch));
	while ((uint32_t)i < sw32(sym->nsyms, file.arch))
	{
		if (symtab[i].n_type & N_STAB && ++i)
			continue;
		type = get_type(sw64(symtab[i].n_value, file.arch),
		symtab[i].n_sect, symtab[i].n_type, sect);
		if (add_symbol(&syms, sw64(symtab[i].n_value, file.arch),
		type, strtab + sw32(symtab[i].n_un.n_strx, file.arch)) < 0
		|| check_size(&file, sw32(sym->symoff, file.arch) + (i + 1)
		* sizeof(struct nlist_64), F_BEGIN) < 0)
			return (-1);
		i++;
	}
	return (dump_symbols(file, &syms));
}

int			handle_mach(t_file file, int ncmds, struct load_command *lc)
{
	t_sect					*sect;
	struct symtab_command	*sym;
	size_t					sym_offset;

	sect = NULL;
	while (ncmds--)
	{
		if (sw32(lc->cmd, file.arch) == LC_SYMTAB
		&& (sym_offset = file.free_size))
			sym = (struct symtab_command*)lc;
		else if (sw32(lc->cmd, file.arch) == LC_SEGMENT_64
		&& get_section_64(file, (struct segment_command_64*)lc, &sect) < 0)
			return (-1);
		else if (sw32(lc->cmd, file.arch) == LC_SEGMENT
		&& get_section(file, (struct segment_command*)lc, &sect) < 0)
			return (-1);
		if (check_size(&file, sw32(lc->cmdsize, file.arch), F_OFFSET) < 0)
			return (-1);
		lc = (void*)((size_t)lc + sw32(lc->cmdsize, file.arch));
	}
	if (check_symtab(file, sym, sym_offset, sect) < 0)
		return (-1);
	free_sect(&sect);
	return (0);
}

int			handle_mach_header(t_file file, struct mach_header *mh)
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
