/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   nm.h                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: clanier <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/12/09 12:37:23 by clanier           #+#    #+#             */
/*   Updated: 2017/12/09 13:28:06 by clanier          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef NM_H

# include <sys/mman.h>
# include <mach-o/loader.h>
# include <mach-o/nlist.h>
# include <fcntl.h>
# include <sys/stat.h>
# include <stdlib.h>
# include <stdbool.h>
# include "libft.h"

typedef				struct s_symbol
{
	char			*name;
	char			type;
	uint64_t		value;
	struct s_symbol	*next;
}					t_symbol;

typedef				struct s_pflags
{
	unsigned char	text_nsect;
	unsigned char	data_nsect;
	unsigned char	bss_nsect;
}					t_pflags;

int a = 0;

#endif
