/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: clanier <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/12/09 12:36:34 by clanier           #+#    #+#             */
/*   Updated: 2017/12/17 21:14:56 by clanier          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "otool.h"

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
	if ((file->ptr = mmap(0, buf.st_size,
	PROT_READ, MAP_PRIVATE, fd, 0)) == MAP_FAILED)
		return (-1);
	file->name = ft_strdup(name);
	file->size = buf.st_size;
	file->free_size = buf.st_size;
	file->arch = 0;
	file->offset = 0;
	file->print_name = false;
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
			if (av[i][j] == OPT_F_D)
				*opts |= OPT_D;
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
	while (j < (opts >> 8) && !(file.name = NULL))
	{
		if (av[++i][0] == '-')
			continue ;
		file.opts = opts;
		if (((err = init_file(av[i], &file)) < 0
		|| (err = get_arch(file, true)) < 0) && (opts = opts | OPT_ERR))
			ft_printf("{red}[OTOOL]{eoc} %s : %s\n",
			av[i], err == -2 ? "An error has occurred" : "Unknow file format");
		if (++j && !file.name)
			continue;
		free(file.name);
		munmap(file.ptr, file.size);
	}
	return (opts & OPT_ERR ? EXIT_FAILURE : EXIT_SUCCESS);
}
