/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   nm.h                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: clanier <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/12/09 12:37:23 by clanier           #+#    #+#             */
/*   Updated: 2017/12/15 19:37:19 by clanier          ###   ########.fr       */
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

# define F_BEGIN	0b01
# define F_OFFSET	0b10

# define OPT_F_MU	'U'
# define OPT_F_U	'u'
# define OPT_F_P	'p'
# define OPT_F_J	'j'
# define OPT_F_R	'r'
# define OPT_MU		0b00001
# define OPT_U		0b00010
# define OPT_P		0b00100
# define OPT_J		0b01000
# define OPT_R		0b10000

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
	long int		size;
	long int		free_size;
	uint8_t			arch;
	uint64_t		offset;
	uint16_t		opts;
}					t_file;

int					get_arch(t_file file, bool print_name);

#endif
