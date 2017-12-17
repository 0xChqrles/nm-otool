/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   nm.h                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: clanier <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/12/09 12:37:23 by clanier           #+#    #+#             */
/*   Updated: 2017/12/17 21:26:47 by clanier          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef OTOOL_H
# define OTOOL_H

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

# define OPT_F_D	'd'
# define OPT_D		0b1

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

typedef struct	s_file
{
	char		*ptr;
	char		*name;
	long int	size;
	long int	free_size;
	uint8_t		arch;
	uint64_t	offset;
	uint16_t	opts;
}				t_file;

typedef struct	s_txt_section
{
	uint32_t	offset;
	uint64_t	size;
	uint64_t	addr;
	bool		find;
}				t_txt_section;

uint8_t			is_valid_arch(uint32_t magic);
uint8_t			is_64_arch(uint32_t magic);
uint8_t			is_cigam_arch(uint32_t magic);
uint8_t			is_mach_arch(uint32_t magic);
int				get_arch(t_file file, bool print_name);
int				check_size(t_file *file, long int size, uint8_t flag);
void			dump_section(t_file file,
				uint32_t offset, uint64_t size, uint64_t addr);
void			print_binary_arch(char *name, cpu_type_t cpu);
int				handle_fat_64(t_file file, uint32_t nfat_arch);
int				handle_fat(t_file file, uint32_t nfat_arch);
int				handle_fat_header(t_file file);
int				handle_mach(t_file file, int ncmds, struct load_command *lc);
int				handle_mach_header(t_file file, struct mach_header *mh);
int				get_section(t_file file,
				struct segment_command *sg, t_txt_section *txt_sect);
int				get_section_64(t_file file,
				struct segment_command_64 *sg, t_txt_section *txt_sect);
int				pexit(char *s);
int				init_file(char *name, t_file *file);
int				get_options(int ac, char **av, uint16_t *opts);
int				main(int ac, char **av);
uint32_t		sw16(uint16_t n, uint8_t arch);
uint32_t		sw32(uint32_t n, uint8_t arch);
uint64_t		sw64(uint64_t n, uint8_t arch);

#endif
