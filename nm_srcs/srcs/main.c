/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: clanier <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/12/09 12:36:34 by clanier           #+#    #+#             */
/*   Updated: 2017/12/09 21:14:04 by clanier          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "nm.h"

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

void		print_symbols(t_symbol **syms)
{
	t_symbol	*tmp;

	tmp = *syms;
	while (tmp)
	{
		if (tmp->type == 'U' || tmp->type == 'u')
			ft_printf("%17c", ' ');
		else
			ft_printf("%016llx ", tmp->value);
		ft_printf("%1c %s\n", tmp->type, tmp->name);
		tmp = tmp->next;
	}
}

char		get_sect(uint16_t n_sect, t_pflags pflags)
{
	if (n_sect == pflags.text_nsect)
		return ('T');
	else if (n_sect == pflags.data_nsect)
		return ('D');
	else if (n_sect == pflags.bss_nsect)
		return ('B');
	else
		return ('S');
}

char		get_type(struct nlist_64 sym, t_pflags pflags)
{
	char		type;
	uint16_t	n_type;

	type = '?';
	n_type = sym.n_type & N_TYPE;
	if (n_type == N_UNDF)
		type = sym.n_value ? 'C' : 'U';
	else if (n_type == N_PBUD)
		type = 'U';
	else if (n_type == N_ABS)
		type = 'A';
	else if (n_type == N_INDR)
		type = 'I';
	else if (n_type == N_SECT)
		type = get_sect(sym.n_sect, pflags);
	if (sym.n_type & N_STAB)
		type = 'Z';
	if (!(sym.n_type & N_EXT) && type != '?')
		type -= 32;
	return (type);
}

void		get_symbols(struct symtab_command *sym, t_pflags pflags, char *ptr)
{
	int				i;
	char			*strtab;
	struct nlist_64	*symtab;
	t_symbol		*syms;
	char			type;

	i = 0;
	syms = NULL;
	symtab = (struct nlist_64*)((size_t)ptr + sym->symoff);
	strtab = (char*)((size_t)ptr + sym->stroff);
	while (i < (int)sym->nsyms)
	{
		if (symtab[i].n_type & N_STAB && ++i)
			continue;
		if (!(type = get_type(symtab[i], pflags))
		|| add_symbol(&syms, symtab[i].n_value, type,
		strtab + symtab[i].n_un.n_strx) < 0)
			return ;
		i++;
	}
	if (!syms)
		return ;
	bubblesort_symbols(&syms);
	print_symbols(&syms);
	free_symbol(&syms);
}

t_pflags		get_pflags_64(struct segment_command_64 *sg)
{
	t_pflags			pflags;
	struct section_64	*s;
	uint32_t			i;

	i = 0;
	s = (struct section_64*)((size_t)sg + sizeof(struct segment_command_64));
	while (i < sg->nsects)
	{
		if (!ft_strcmp((s + i)->sectname, SECT_TEXT)
		&& !ft_strcmp((s + i)->segname, SEG_TEXT))
			pflags.text_nsect = i + 1;
		else if (!ft_strcmp((s + i)->sectname, SECT_DATA)
		&& !ft_strcmp((s + i)->segname, SEG_DATA))
			pflags.data_nsect = i + 1;
		else if (!ft_strcmp((s + i)->sectname, SECT_BSS)
		&& !ft_strcmp((s + i)->segname, SEG_DATA))
			pflags.bss_nsect = i + 1;
		i++;
	}
	return (pflags);
}

void			handle_64(char *ptr)
{
	int						i;
	t_pflags				pflags;
	struct mach_header_64	*header;
	struct load_command		*lc;
	struct symtab_command	*sym;

	i = 0;
	header = (struct mach_header_64*)ptr;
	lc = (struct load_command*)((size_t)ptr + sizeof(*header));
	while (i++ < (int)header->ncmds)
	{
		if (lc->cmd == LC_SYMTAB)
			sym = (struct symtab_command*)lc;
		else if (lc->cmd == LC_SEGMENT_64)
			pflags = get_pflags_64((struct segment_command_64*)lc);
		lc = (void*)((size_t)lc + lc->cmdsize);
	}
	get_symbols(sym, pflags, ptr);
}

int				pexit(char *s)
{
	ft_printf(s);
	return (EXIT_FAILURE);
}

unsigned int	get_magic(char *ptr)
{
	unsigned int	magic;

	magic = *(int*)ptr;
	return (magic);
}

char			*get_binary(char *file)
{
	char		*ptr;
	int			fd;
	struct stat	buf;

	if ((fd = open(file, O_RDONLY)) < 1 || fstat(fd, &buf) < 0)
	{
		ft_printf("{red}[NM]{eoc} %s : Invalid binary\n", file);
		return (NULL);
	}
	ptr = mmap(0, buf.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
	if (ptr == MAP_FAILED)
	{
		ft_printf("{red}[NM]{eoc} %s : Invalid binary\n", file);
		return (NULL);
	}
	return (ptr);
}

int				main(int ac, char **av)
{
	char	*ptr;

	if (ac != 2)
		return (pexit("{red}[NM]{eoc} Invalid agruments\n"));
	(void)av;
	if (!(ptr = get_binary(av[1])))
		return (EXIT_FAILURE);
	if (get_magic(ptr) == MH_MAGIC_64)
		handle_64(ptr);
}
