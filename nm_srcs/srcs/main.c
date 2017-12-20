/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: clanier <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/12/09 12:36:34 by clanier           #+#    #+#             */
/*   Updated: 2017/12/17 20:38:18 by clanier          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "nm.h"

int	pexit(char *s)
{
	ft_printf(s);
	return (EXIT_FAILURE);
}

int	init_file(char *name, t_file *file)
{
	int			fd;
	struct stat	buf;

	if ((fd = open(name, O_RDONLY)) < 1 || fstat(fd, &buf) < 0)
		return (-1);
	if ((file->ptr = mmap(0, buf.st_size,
	PROT_READ, MAP_PRIVATE, fd, 0)) == MAP_FAILED)
		return (-1);
	close(fd);
	file->name = ft_strdup(name);
	file->size = buf.st_size;
	file->free_size = buf.st_size;
	file->arch = 0;
	file->offset = 0;
	file->print_name = false;
	return (0);
}

int	get_options(int ac, char **av, uint16_t *opts)
{
	int	i;
	int	j;

	i = 0;
	while (++i < ac && !(j = 0))
	{
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

int	main(int ac, char **av)
{
	t_file		file;
	uint16_t	opts;
	int			err;
	int			i;
	int			j;

	i = 0;
	if ((j = get_options(ac, av, &opts)) < 0)
		return (pexit("{red}[NM]{eoc} Invalid agruments\n"));
	while (j < (opts >> 8) && !(file.name = NULL))
	{
		if (av[++i][0] == '-')
			continue ;
		file.opts = opts;
		if (((err = init_file(av[i], &file)) < 0
		|| (err = get_arch(file, true)) < 0) && (opts = opts | OPT_ERR))
			ft_printf("%c{red}[NM]{eoc} %s : %s\n",
			(opts >> 8) > 1 ? '\n' : '\0',
			av[i], err == -2 ? "An error has occurred" : "Unknow file format");
		if (++j && !file.name)
			continue;
		free(file.name);
		munmap(file.ptr, file.size);
	}
	return (opts & OPT_ERR ? EXIT_FAILURE : EXIT_SUCCESS);
}
