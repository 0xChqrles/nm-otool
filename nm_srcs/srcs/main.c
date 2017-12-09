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

void	print_symtab(int nsyms, int symoff, int stroff, char *ptr)
{
	int				i;
	char			*strtab;
	struct nlist_64	*symtab;

	symtab = (struct nlist_64*)((size_t)ptr + symoff);
	strtab = (char*)((size_t)ptr + stroff);
	i = 0;
	while (i <nsyms)
	{
		if (symtab[i].n_type & N_STAB && ++i)
			continue;
		printf("%s\n", strtab + symtab[i].n_un.n_strx);
		i++;
	}
}

void			handle_64(char *ptr)
{
	int						ncmds;
	int						i;
	struct mach_header_64	*header;
	struct load_command		*lc;
	struct symtab_command	*sym;

	header = (struct mach_header_64*)ptr;
	ncmds = header->ncmds;
	lc = (struct load_command*)((size_t)ptr + sizeof(*header));
	i = 0;
	while (i++ < ncmds)
	{
		if (lc->cmd == LC_SYMTAB)
		{
			sym = (struct symtab_command*)lc;
			print_symtab(sym->nsyms, sym->symoff, sym->stroff, ptr);
			break ;
		}
		lc = (void*)((size_t)lc + lc->cmdsize);
	}
}

int				pexit(char *s)
{
	ft_printf(s);
	return (EXIT_FAILURE);
}

unsigned int	getMagic(char *ptr)
{
	unsigned int	magic;

	magic = *(int*)ptr;
	return (magic);
}

char			*getBinary(char *file)
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
	if (!(ptr = getBinary(av[1])))
		return (EXIT_FAILURE);
	if (getMagic(ptr) == MH_MAGIC_64)
		handle_64(ptr);
}
