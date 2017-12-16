/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: clanier <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/12/09 12:36:34 by clanier           #+#    #+#             */
/*   Updated: 2017/12/15 19:47:24 by clanier          ###   ########.fr       */
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

void		swap_symbols(t_symbol **syms, t_symbol **tmp, t_symbol **next, t_symbol **prev)
{
	if (*prev)
		(*prev)->next = (*tmp)->next;
	else
		*syms = (*tmp)->next;
	(*next) = (*tmp)->next->next;
	(*tmp)->next->next = *tmp;
	(*tmp)->next = *next;
	*tmp = (*tmp)->next;	
}

void		reverse_symbols(t_symbol **syms)
{
	t_symbol	*prev;
	t_symbol	*tmp;
	t_symbol	*next;

	if (!*syms || !(*syms)->next)
		return ;
	tmp = *syms;
	next = tmp->next;
	prev = NULL;
	while (next)
	{
		tmp->next = prev;
		prev = tmp;
		tmp = next;
		next = next->next;
	}
	tmp->next = prev;
	*syms = tmp;
}

void		bubblesort_symbols(t_symbol **syms)
{
	t_symbol	*prev;
	t_symbol	*tmp;
	t_symbol	*next;
	bool		isSort;
	char		cmp;

	isSort = false;
	while (!isSort && (isSort = true))
	{
		tmp = *syms;
		prev = NULL;
		while (tmp && tmp->next)
		{
			cmp = ft_strcmp(tmp->name, tmp->next->name);
			if ((cmp > 0 || (!cmp && tmp->type == 'I')) && !(isSort = false))
				swap_symbols(syms, &tmp, &next, &prev);
			prev = tmp;
			tmp = tmp ? tmp->next : NULL;
		}
	}
}

void		print_symbols(t_file file, t_symbol **syms)
{
	t_symbol	*tmp;

	tmp = *syms;
	while (tmp)
	{
		if (((tmp->type == 'U' || tmp->type == 'u') && file.opts & OPT_MU)
		|| (tmp->type != 'U' && tmp->type != 'u' && file.opts & OPT_U))
		{
			tmp = tmp->next;
			continue ;
		}
		if (!(file.opts & OPT_U) && !(file.opts & OPT_J))
		{
			if (tmp->type == 'U' || tmp->type == 'u')
				ft_printf(file.arch & ARCH_64 ? "%17c" : "%9c", ' ');
			else
				ft_printf(file.arch & ARCH_64 ? "%016llx " : "%08llx ", tmp->value);
			ft_printf("%1c ", tmp->type);
		}
		ft_printf("%s\n", tmp->name);
		tmp = tmp->next;
	}
}

int		dump_symbols(t_file file, t_symbol **syms)
{
	if (!(*syms))
		return (-1);
	if (!(file.opts & OPT_P))
	{
		bubblesort_symbols(syms);
		if ((file.opts & OPT_R))
			reverse_symbols(syms);
	}
	print_symbols(file, syms);
	free_symbol(syms);
	return (0);
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

int			check_size(t_file *file, long int size, uint8_t flag)
{
	if (flag & F_BEGIN && file->size - size < 0)
		return (-1);
	if (flag & F_OFFSET && (file->free_size -= size) < 0)
		return (-1);
	return (0);
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

int			get_section(t_file file, struct segment_command *sg, t_sect **sect)
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

int			get_section_64(t_file file, struct segment_command_64 *sg, t_sect **sect)
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

int		get_symbols(t_file file, struct symtab_command *sym, t_sect *sect)
{
	int				i;
	char			*strtab;
	struct nlist	*symtab;
	t_symbol		*syms;
	char			type;

	i = 0;
	syms = NULL;
	if ((check_size(&file, sw32(sym->symoff, file.arch), F_BEGIN) < 0)
	|| (check_size(&file, sw32(sym->stroff, file.arch), F_BEGIN) < 0))
		return (-1);
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
		if (check_size(&file, sw32(sym->symoff, file.arch) + (i + 1) * sizeof(struct nlist), F_BEGIN) < 0)
			return (-1);
		i++;
	}
	return (dump_symbols(file, &syms));
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
	if ((check_size(&file, sw32(sym->symoff, file.arch), F_BEGIN) < 0)
	|| (check_size(&file, sw32(sym->stroff, file.arch), F_BEGIN) < 0))
		return (-1);
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
		if (check_size(&file, sw32(sym->symoff, file.arch) + (i + 1) * sizeof(struct nlist_64), F_BEGIN) < 0)
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
		if (sw32(lc->cmd, file.arch) == LC_SYMTAB && (sym_offset = file.free_size))
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
	file.free_size = file.size - sym_offset + sizeof(struct load_command) - sizeof(struct symtab_command);
	if (file.free_size <= 0)
		return (-1);
	if ((file.arch & ARCH_64 && get_symbols_64(file, sym, sect) < 0)
	|| (file.arch & ARCH_32 && get_symbols(file, sym, sect) < 0))
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

	if (nfat_arch < 1 || check_size(&file, sizeof(struct fat_arch_64) * nfat_arch, F_OFFSET) < 0)
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
		if ((!i && get_arch(file, false) < 0) || ((uint32_t)i == (nfat_arch + 1) && get_arch(file, true) < 0))
			return (-1);
		fat = (struct fat_arch_64*)((size_t)fat + sizeof(struct fat_arch_64));
	}
	return (0);
}

int			handle_fat(t_file file, uint32_t nfat_arch)
{
	struct fat_arch	*fat;
	int				i;

	if (nfat_arch < 1 || check_size(&file, sizeof(struct fat_arch) * nfat_arch, F_OFFSET) < 0)
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
		if ((!i && get_arch(file, false) < 0) || ((uint32_t)i == (nfat_arch + 1) && get_arch(file, true) < 0))
			return (-1);
		fat = (struct fat_arch*)((size_t)fat + sizeof(struct fat_arch));
	}
	return (0);
}

int			handle_fat_header(t_file file)
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

int			get_arch(t_file file, bool print_name)
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
		ft_printf("\n%s:\n", file.name);
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
	file->free_size = buf.st_size;
	file->arch = 0;
	file->offset = 0;
	return (0);
}

int			get_options(int ac, char **av, uint16_t *opts)
{
	int	i;
	int	j;

	i = 0;
	while (++i < ac)
	{
		j = 0;
		while (av[i][0] == '-' && av[i][++j])
		{
			if (av[i][j] == OPT_F_MU)
				*opts |= OPT_MU;
			else if (av[i][j] == OPT_F_U)
				*opts |= OPT_U;
			else if (av[i][j] == OPT_F_P)
				*opts |= OPT_P;
			else if (av[i][j] == OPT_F_J)
				*opts |= OPT_J;
			else if (av[i][j] == OPT_F_R)
				*opts |= OPT_R;
			else
				return (-1);
		}
		if (!j)
			*opts = (((*opts >> 8) + 1) << 8 | (*opts & 0xff));
	}
	return (0);
}

int			main(int ac, char **av)
{
	t_file		file;
	uint16_t	opts;
	int			err;
	int			i;
	int			j;

	i = 0;
	j = 0;
	if (get_options(ac, av, &opts) < 0)
		return (pexit("{red}[NM]{eoc} Invalid agruments\n"));
	while (j < (opts >> 8))
	{
		if (av[++i][0] == '-')
			continue ;
		file.opts = opts;
		if ((err = init_file(av[i], &file)) < 0
		|| (err = get_arch(file, true)) < 0)
			return (pexit(err == -2 ? "{red}[NM]{eoc} Unknow file format\n"
			: "{red}[NM]{eoc} An error has occurred\n"));
		free(file.name);
		munmap(file.ptr, file.size);
		j++;
	}
}
