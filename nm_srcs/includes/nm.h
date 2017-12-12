/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   nm.h                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: clanier <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/12/09 12:37:23 by clanier           #+#    #+#             */
/*   Updated: 2017/12/12 16:46:24 by clanier          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef NM_H

# define ARCH_VAL	0b00000001
# define MACH_O		0b00000010
# define FAT		0b00000100
# define AR			0b00001000
# define ARCH_64	0b00010000
# define ARCH_32	0b00100000
# define CIGAM		0b01000000
# define MAGIC		0b10000000

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

typedef				struct s_symbol
{
	char			*name;
	char			type;
	uint64_t		value;
	struct s_symbol	*next;
}					t_symbol;

typedef				struct s_sect
{
	char			type;
	int				n;
	struct s_sect	*next;
}					t_sect;

typedef				struct s_file
{
	char			*ptr;
	char			*name;
	size_t			size;
	uint8_t			arch;
	uint64_t		offset;
}					t_file;

int					get_arch(t_file file);

#endif
