/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   nm.h                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: clanier <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/12/09 12:37:23 by clanier           #+#    #+#             */
/*   Updated: 2017/12/16 18:35:16 by clanier          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef OTOOL_H

# define ARCH_VAL	0b00000001
# define MACH_O		0b00000010
# define FAT		0b00000100
# define AR			0b00001000
# define ARCH_64	0b00010000
# define ARCH_32	0b00100000
# define CIGAM		0b01000000
# define MAGIC		0b10000000

# define F_BEGIN	0b01
# define F_OFFSET	0b10

# include <sys/mman.h>
# include <mach-o/loader.h>
# include <mach-o/fat.h>
# include <mach-o/nlist.h>
# include <fcntl.h>
# include <sys/stat.h>
# include <stdlib.h>
# include <ar.h>
# include <stdbool.h>
# include "libft.h"

typedef				struct s_file
{
	char			*ptr;
	char			*name;
	long int		size;
	long int		free_size;
	uint8_t			arch;
	uint64_t		offset;
	uint16_t		opts;
}					t_file;

int					get_arch(t_file file, bool print_name);

#endif
