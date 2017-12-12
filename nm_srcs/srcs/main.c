/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: clanier <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/12/09 12:36:34 by clanier           #+#    #+#             */
/*   Updated: 2017/12/12 22:49:59 by clanier          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "nm.h"

uint32_t	sw16(uint16_t n, uint8_t arch)
{
	if (arch & MAGIC)
		return (n);
	return ((n >> 8) | (n << 8));
}

uint32_t	sw32(uint32_t n, uint8_t arch)
{
	if (arch & MAGIC)
		return (n);
	n = ((n >> 8) & 0xFF00FF) | ((n << 8) & 0xFF00FF00);
	return ((n >> 16) | (n << 16));
}

uint64_t	sw64(uint64_t n, uint8_t arch)
{
	if (arch & MAGIC)
		return (n);
	n = ((n >> 8) & 0xFF00FF00FF00FF) | ((n << 8) & 0xFF00FF00FF00FF00);
	n = ((n >> 16) & 0xFFFF0000FFFF) | ((n << 16) & 0x0000FFFF0000FFFF);
	return ((n >> 32) | (n << 32));
}

t_symbol	*new_symbol(uint64_t value, char type, char *name)
{
	t_symbol	*new;

	if (!(new = malloc(sizeof(t_symbol))))
		return (NULL);
	new->next = NULL;
	new->value = value;
	new->name = name;
	new->type = type;
	return (new);
}

int			add_symbol(t_symbol **syms, uint64_t value, char type, char *name)
{
	t_symbol	*tmp;

	if (!(*syms))
	{
		if (!(*syms = new_symbol(value, type, name)))
			return (-1);
		return (0);
	}
	tmp = *syms;
	while (tmp->next)
		tmp = tmp->next;
	if (!(tmp->next = new_symbol(value, type, name)))
		return (-1);
	return (0);
}

void		free_symbol(t_symbol **syms)
{
	t_symbol	*tmp;
	t_symbol	*tmp2;

	tmp = *syms;
	while (tmp)
	{
		tmp2 = tmp;
		tmp = tmp->next;
		free(tmp);
	}
}

void		bubblesort_symbols(t_symbol **syms)
{
	t_symbol	*prev;
	t_symbol	*tmp;
	t_symbol	*next;
	bool		isSort;

	isSort = false;
	while (!isSort && (isSort = true))
	{
		tmp = *syms;
		prev = NULL;
		while (tmp && tmp->next)
		{
			if (ft_strcmp(tmp->name, tmp->next->name) > 0 && !(isSort = false))
			{
				if (prev)
					prev->next = tmp->next;
			   	else
					*syms = tmp->next;
				next = tmp->next->next;
				tmp->next->next = tmp;
				tmp->next = next;
				tmp = tmp->next;
			}
			prev = tmp;
			tmp = tmp ? tmp->next : NULL;
		}
	}
}

void		print_symbols(t_symbol **syms, char arch)
{
	t_symbol	*tmp;

	tmp = *syms;
	while (tmp)
	{
		if (tmp->type == 'U' || tmp->type == 'u')
			ft_printf(arch & ARCH_64 ? "%17c" : "%9c", ' ');
		else
			ft_printf(arch & ARCH_64 ? "%016llx " : "%08llx ", tmp->value);
		ft_printf("%1c %s\n", tmp->type, tmp->name);
		tmp = tmp->next;
	}
}

void		*new_sect(char type, int n)
{
	t_sect	*new;

	if (!(new = malloc(sizeof(t_sect))))
		return (NULL);
	new->type = type;
	new->n = n;
	new->next = NULL;
	return (new);
}

int			add_sect(t_sect **sect, char type)
{
	t_sect	*tmp;
	int		n;

	n = 1;
	if (!(*sect))
	{
		if (!(*sect = new_sect(type, n)))
			return (-1);
		return (0);
	}
	tmp = *sect;
	while (++n && tmp->next)
		tmp = tmp->next;
	if (!(tmp->next = new_sect(type, n)))
		return (-1);
	return (0);
}

void		free_sect(t_sect **sect)
{
	t_sect	*tmp;
	t_sect	*tmp2;

	tmp = *sect;
	while (tmp)
	{
		tmp2 = tmp;
		tmp = tmp->next;
		free(tmp);
	}
}

int			handle_sectname(char *sectname, t_sect **sect)
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

int			get_section(struct segment_command *sg, t_sect **sect, uint8_t arch)
{
	struct section	*s;
	uint32_t		i;

	i = 0;
	s = (struct section*)((size_t)sg + sizeof(struct segment_command));
	while (i++ < sw32(sg->nsects, arch))
	{
		if (handle_sectname(s->sectname, sect) < 0)
			return (-1);
		s = (struct section*)((size_t)s + sizeof(struct section));
	}
	return (0);
}

int			get_section_64(struct segment_command_64 *sg, t_sect **sect, uint8_t arch)
{
	struct section_64	*s;
	uint32_t			i;

	i = 0;
	s = (struct section_64*)((size_t)sg + sizeof(struct segment_command_64));
	while (i++ < sw32(sg->nsects, arch))
	{
		if (handle_sectname(s->sectname, sect) < 0)
			return (-1);
		s = (struct section_64*)((size_t)s + sizeof(struct section_64));
	}
	return (0);
}

char		get_secttype(t_sect **sect, uint16_t n_sect)
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

char		get_type(uint64_t n_value, uint8_t n_sect, uint8_t n_type, t_sect *sect)
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

int		dump_symbols(t_symbol **syms, uint8_t arch)
{
	if (!(*syms))
		return (-1);
	bubblesort_symbols(syms);
	print_symbols(syms, arch);
	free_symbol(syms);
	return (0);
}

int		get_symbols(t_file file, struct symtab_command *sym, t_sect *sect)
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
		if ((symtab[i].n_type & N_TYPE) == N_SECT
		&& sw16(symtab[i].n_desc, file.arch) == N_ARM_THUMB_DEF)
			symtab[i].n_value = sw32(sw32(symtab[i].n_value, file.arch) | 1, file.arch);
		type = get_type(sw32(symtab[i].n_value, file.arch),
		symtab[i].n_sect, symtab[i].n_type, sect);
		if (add_symbol(&syms, sw32(symtab[i].n_value, file.arch),
		type, strtab + sw32(symtab[i].n_un.n_strx, file.arch)) < 0)
			return (-1);
		i++;
	}
	return (dump_symbols(&syms, file.arch));
}

int		get_symbols_64(t_file file, struct symtab_command *sym, t_sect *sect)
{
	int				i;
	char			*strtab;
	struct nlist_64	*symtab;
	t_symbol		*syms;
	char			type;

	i = 0;
	syms = NULL;
	symtab = (struct nlist_64*)((size_t)file.ptr + sw32(sym->symoff, file.arch));
	strtab = (char*)((size_t)file.ptr + sw32(sym->stroff, file.arch));
	while ((uint32_t)i < sw32(sym->nsyms, file.arch))
	{
		if (symtab[i].n_type & N_STAB && ++i)
			continue;
		type = get_type(sw64(symtab[i].n_value, file.arch),
		symtab[i].n_sect, symtab[i].n_type, sect);
		if (add_symbol(&syms, sw64(symtab[i].n_value, file.arch),
		type, strtab + sw32(symtab[i].n_un.n_strx, file.arch)) < 0)
			return (-1);
		i++;
	}
	return (dump_symbols(&syms, file.arch));
}

int			handle_mach(t_file file, int ncmds, struct load_command *lc)
{
	t_sect					*sect;
	struct symtab_command	*sym;

	sect = NULL;
	while (ncmds--)
	{
		if (sw32(lc->cmd, file.arch) == LC_SYMTAB)
			sym = (struct symtab_command*)lc;
		else if (sw32(lc->cmd, file.arch) == LC_SEGMENT_64
		&& get_section_64((struct segment_command_64*)lc, &sect, file.arch) < 0)
			return (-1);
		else if (sw32(lc->cmd, file.arch) == LC_SEGMENT
		&& get_section((struct segment_command*)lc, &sect, file.arch) < 0)
			return (-1);
		lc = (void*)((size_t)lc + sw32(lc->cmdsize, file.arch));
	}
	if ((file.arch & ARCH_64 && get_symbols_64(file, sym, sect) < 0)
	|| (file.arch & ARCH_32 && get_symbols(file, sym, sect) < 0))
		return (-1);
	free_sect(&sect);
	return (0);
}

int			handle_mach_header(t_file file, struct mach_header *mh)
{
	struct mach_header_64	*mh_64;

	if (file.arch & ARCH_32)
		return (handle_mach(file, sw32(mh->ncmds, file.arch),
		(struct load_command*)((size_t)file.ptr + sizeof(*mh))));
	else if (file.arch & ARCH_64)
	{
		mh_64 = (struct mach_header_64*)file.ptr;
		return (handle_mach(file, sw32(mh_64->ncmds, file.arch),
		(struct load_command*)((size_t)file.ptr + sizeof(*mh_64))));
	}
	return (-1);
}

void		print_binary_arch(char *name, cpu_type_t cpu)
{
	ft_printf("\n%s (for architecture", name);
	if (cpu == CPU_TYPE_X86)
		ft_printf(" i386");
	else if (cpu == CPU_TYPE_POWERPC)
		ft_printf(" ppc");
	ft_printf("):\n");
}

int			handle_fat_64(t_file file, uint32_t nfat_arch)
{
	struct fat_arch_64	*fat;
	int					i;

	if (nfat_arch < 1)
		return (-1);
	i = nfat_arch;
	fat = (struct fat_arch_64*)((size_t)file.ptr + sizeof(struct fat_header));
	while (sw32(fat->cputype, file.arch) != CPU_TYPE_X86_64 && i--)
		fat = (struct fat_arch_64*)((size_t)fat + sizeof(struct fat_arch_64));
	fat = (struct fat_arch_64*)((size_t)file.ptr + sizeof(struct fat_header));
	while (nfat_arch--)
	{
		if (!i)
			print_binary_arch(file.name, sw32(fat->cputype, file.arch));
		file.offset = sw64(fat->offset, file.arch);
		if ((!i && get_arch(file) < 0)
		|| (sw32(fat->cputype, file.arch) == CPU_TYPE_X86_64 && get_arch(file) < 0))
			return (-1);
		fat = (struct fat_arch_64*)((size_t)fat + sizeof(struct fat_arch_64));
	}
	return (0);
}

int			handle_fat(t_file file, uint32_t nfat_arch)
{
	struct fat_arch	*fat;
	int				i;

	if (nfat_arch < 1)
		return (-1);
	i = nfat_arch;
	fat = (struct fat_arch*)((size_t)file.ptr + sizeof(struct fat_header));
	while (sw32(fat->cputype, file.arch) != CPU_TYPE_X86_64 && --i)
		fat = (struct fat_arch*)((size_t)fat + sizeof(struct fat_arch));
	fat = (struct fat_arch*)((size_t)file.ptr + sizeof(struct fat_header));
	while (nfat_arch--)
	{
		if (!i)
			print_binary_arch(file.name, sw32(fat->cputype, file.arch));
		file.offset = sw32(fat->offset, file.arch);
		if ((!i && get_arch(file) < 0)
		|| (sw32(fat->cputype, file.arch) == CPU_TYPE_X86_64 && get_arch(file) < 0))
			return (-1);
		fat = (struct fat_arch*)((size_t)fat + sizeof(struct fat_arch));
	}
	return (0);
}

int			handle_fat_header(t_file file)
{
	struct fat_header	*fh;

	fh = (struct fat_header*)file.ptr;
	if (file.arch & ARCH_32)
		return (handle_fat(file, sw32(fh->nfat_arch, file.arch)));
	else if (file.arch & ARCH_64)
		return (handle_fat_64(file, sw32(fh->nfat_arch, file.arch)));
	return (-1);
}

uint8_t		is_valid_arch(uint32_t magic)
{
	return (magic == MH_MAGIC_64 || magic == MH_CIGAM_64
	|| magic == FAT_MAGIC_64 || magic == FAT_CIGAM_64
	|| magic == MH_MAGIC || magic == MH_CIGAM
	|| magic == FAT_MAGIC || magic == FAT_CIGAM ? ARCH_VAL : 0);
}

uint8_t		is_64_arch(uint32_t magic)
{
	return (magic == MH_MAGIC_64 || magic == MH_CIGAM_64
	|| magic == FAT_MAGIC_64 || magic == FAT_CIGAM_64 ? ARCH_64 : ARCH_32);
}

uint8_t		is_cigam_arch(uint32_t magic)
{
	return (magic == MH_CIGAM_64 || magic == MH_CIGAM
	|| magic == FAT_CIGAM || magic == FAT_CIGAM_64 ? CIGAM : MAGIC);
}

uint8_t		is_mach_arch(uint32_t magic)
{
	return (magic == MH_MAGIC_64 || magic == MH_MAGIC
	|| magic == MH_CIGAM || magic == MH_CIGAM_64 ? MACH_O : FAT);
}

int			get_arch(t_file file)
{
	struct mach_header	*mh;

	if (file.offset)
		file.ptr = (char*)((size_t)file.ptr + file.offset);
	mh = (struct mach_header*)file.ptr;
	file.arch = is_valid_arch(mh->magic) | is_mach_arch(mh->magic)
	| is_64_arch(mh->magic) | is_cigam_arch(mh->magic);
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

int			pexit(char *s)
{
	ft_printf(s);
	return (EXIT_FAILURE);
}

int			init_file(char *name, t_file *file)
{
	int			fd;
	struct stat	buf;

	if ((fd = open(name, O_RDONLY)) < 1 || fstat(fd, &buf) < 0)
		return (-1);
	if ((file->ptr = mmap(0, buf.st_size, PROT_READ, MAP_PRIVATE, fd, 0)) == MAP_FAILED)
		return (-1);
	file->name = ft_strdup(name);
	file->size = buf.st_size;
	file->arch = 0;
	file->offset = 0;
	return (0);
}

int			main(int ac, char **av)
{
	t_file	file;
	int		err;

	if (ac != 2)
		return (pexit("{red}[NM]{eoc} Invalid agruments\n"));
	if (init_file(av[1], &file) < 0)
		return (pexit("{red}[NM]{eoc} An error has occurred\n"));
	if ((err = get_arch(file)) < 0)
	{
		if (err == -2)
			return (pexit("{red}[NM]{eoc} Unknow file format\n"));
		else
			return (pexit("{red}[NM]{eoc} An error has occurred\n"));
	}
	free(file.name);
	munmap(file.ptr, file.size);
}
