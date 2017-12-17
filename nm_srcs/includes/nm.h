/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   nm.h                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: clanier <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/12/09 12:37:23 by clanier           #+#    #+#             */
/*   Updated: 2017/12/17 20:55:36 by clanier          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef NM_H
# define NM_H

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

typedef struct		s_symbol
{
	char			*name;
	char			type;
	uint64_t		value;
	struct s_symbol	*next;
}					t_symbol;

typedef struct		s_sect
{
	char			type;
	int				n;
	struct s_sect	*next;
}					t_sect;

typedef struct		s_file
{
	char			*ptr;
	char			*name;
	long int		size;
	long int		free_size;
	uint8_t			arch;
	uint64_t		offset;
	uint16_t		opts;
}					t_file;

uint8_t				is_valid_arch(uint32_t magic);
uint8_t				is_64_arch(uint32_t magic);
uint8_t				is_cigam_arch(uint32_t magic);
uint8_t				is_mach_arch(uint32_t magic);
int					get_arch(t_file file, bool print_name);
int					check_size(t_file *file, long int size, uint8_t flag);
int					check_symtab(t_file file, struct symtab_command *sym,
					size_t sym_offset, t_sect *sect);
void				print_binary_arch(char *name, cpu_type_t cpu);
int					handle_fat_64(t_file file, uint32_t nfat_arch);
int					handle_fat(t_file file, uint32_t nfat_arch);
int					handle_fat_header(t_file file);
uint32_t			get_nvalue_32(struct nlist sym, uint8_t arch);
int					handle_mach(t_file file,
					int ncmds, struct load_command *lc);
int					handle_mach_header(t_file file, struct mach_header *mh);
int					handle_sectname(char *sectname, t_sect **sect);
int					get_section(t_file file,
					struct segment_command *sg, t_sect **sect);
int					get_section_64(t_file file,
					struct segment_command_64 *sg, t_sect **sect);
void				print_symbols(t_file file, t_symbol **syms);
int					dump_symbols(t_file file, t_symbol **syms);
int					get_symbols(t_file file,
					struct symtab_command *sym, t_sect *sect);
int					get_symbols_64(t_file file,
					struct symtab_command *sym, t_sect *sect);
char				get_secttype(t_sect **sect, uint16_t n_sect);
char				get_type(uint64_t n_value,
					uint8_t n_sect, uint8_t n_type, t_sect *sect);
int					pexit(char *s);
int					init_file(char *name, t_file *file);
int					get_options(int ac, char **av, uint16_t *opts);
int					main(int ac, char **av);
void				*new_sect(char type, int n);
int					add_sect(t_sect **sect, char type);
void				free_sect(t_sect **sect);
void				swap_symbols(t_symbol **syms, t_symbol **tmp,
					t_symbol **next, t_symbol **prev);
void				reverse_symbols(t_symbol **syms);
void				bubblesort_symbols(t_symbol **syms);
uint32_t			sw16(uint16_t n, uint8_t arch);
uint32_t			sw32(uint32_t n, uint8_t arch);
uint64_t			sw64(uint64_t n, uint8_t arch);
t_symbol			*new_symbol(uint64_t value, char type, char *name);
int					add_symbol(t_symbol **syms,
					uint64_t value, char type, char *name);
void				free_symbol(t_symbol **syms);

#endif
