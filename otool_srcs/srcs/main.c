/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: clanier <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/12/09 12:36:34 by clanier           #+#    #+#             */
/*   Updated: 2017/12/16 20:43:37 by clanier          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "otool.h"

uint32_t	sw16(uint16_t n, uint8_t arch)
{
	if (arch & MAGIC)
		return (n);
	return ((n >> 8) | (n << 8));
}

uint32_t	sw32(uint32_t n, uint8_t arch)
{
	if (arch & MAGIC)
		return (n);
	n = ((n >> 8) & 0xFF00FF) | ((n << 8) & 0xFF00FF00);
	return ((n >> 16) | (n << 16));
}

uint64_t	sw64(uint64_t n, uint8_t arch)
{
	if (arch & MAGIC)
		return (n);
	n = ((n >> 8) & 0xFF00FF00FF00FF) | ((n << 8) & 0xFF00FF00FF00FF00);
	n = ((n >> 16) & 0xFFFF0000FFFF) | ((n << 16) & 0x0000FFFF0000FFFF);
	return ((n >> 32) | (n << 32));
}

int			check_size(t_file *file, long int size, uint8_t flag)
{
	if (flag & F_BEGIN && file->size - size < 0)
		return (-1);
	if (flag & F_OFFSET && (file->free_size -= size) < 0)
		return (-1);
	return (0);
}

int			dump_section(t_file file, uint32_t offset, uint64_t size, uint64_t addr)
{
	uint64_t	i;

	if (check_size(&file, offset + size, F_BEGIN) < 0)
		return (-1);
	i = 0;
	file.ptr = (char*)((size_t)file.ptr + offset);
	ft_printf("Contents of (__TEXT,__text) section\n");
	ft_printf(file.arch & ARCH_64 ? "%016lx\t" : "%08lx\t", addr);
	while (i++ < size)
	{
		ft_printf("%02hhx ", (uint8_t)*(file.ptr));
		if (!(i % 16) && i < size)
			ft_printf(file.arch & ARCH_64 ? "\n%016lx\t" : "\n%08lx\t", addr + i);
		file.ptr++;
	}
	ft_printf("\n");
	return (0);
}

int			get_section(t_file file, struct segment_command *sg)
{
	struct section	*s;
	uint32_t		i;

	i = 0;
	s = (struct section*)((size_t)sg + sizeof(struct segment_command));
	if (check_size(&file, (long int)sizeof(struct segment_command)
	- (long int)sw32(sg->cmdsize, file.arch)
	+ (long int)sizeof(struct section), F_OFFSET) < 0)
		return (-1);
	while (i++ < sw32(sg->nsects, file.arch))
	{
		if (!ft_strcmp(s->sectname, SECT_TEXT))
			return (dump_section(file, s->offset, s->size, s->addr));
		if (check_size(&file, sizeof(struct section), F_OFFSET) < 0)
			return (-1);
		s = (struct section*)((size_t)s + sizeof(struct section));
	}
	return (0);
}

int			get_section_64(t_file file, struct segment_command_64 *sg)
{
	struct section_64	*s;
	uint32_t			i;

	i = 0;
	s = (struct section_64*)((size_t)sg + sizeof(struct segment_command_64));
	if (check_size(&file, (long int)sizeof(struct segment_command_64)
	- (long int)sw32(sg->cmdsize, file.arch)
	+ (long int)sizeof(struct section_64), F_OFFSET) < 0)
		return (-1);
	while (i++ < sw32(sg->nsects, file.arch))
	{
		if (!ft_strcmp(s->sectname, SECT_TEXT))
			return (dump_section(file, s->offset, s->size, s->addr));
		if (check_size(&file, sizeof(struct section_64), F_OFFSET) < 0)
			return (-1);
		s = (struct section_64*)((size_t)s + sizeof(struct section_64));
	}
	return (0);
}

int			handle_mach(t_file file, int ncmds, struct load_command *lc)
{
	while (ncmds--)
	{
		if (sw32(lc->cmd, file.arch) == LC_SEGMENT_64
		&& get_section_64(file, (struct segment_command_64*)lc) < 0)
			return (-1);
		if (sw32(lc->cmd, file.arch) == LC_SEGMENT
		&& get_section(file, (struct segment_command*)lc) < 0)
			return (-1);
		if (check_size(&file, sw32(lc->cmdsize, file.arch), F_OFFSET) < 0)
			return (-1);
		lc = (void*)((size_t)lc + sw32(lc->cmdsize, file.arch));
	}
	return (0);
}

int			handle_mach_header(t_file file, struct mach_header *mh)
{
	struct mach_header_64	*mh_64;

	if (check_size(&file, sizeof(struct load_command), F_OFFSET) < 0)
		return (-1);
	if (file.arch & ARCH_32)
	{
		if (check_size(&file, sizeof(struct mach_header_64), F_OFFSET) < 0)
			return (-1);
		return (handle_mach(file, sw32(mh->ncmds, file.arch),
		(struct load_command*)((size_t)file.ptr + sizeof(*mh))));
	}
	else if (file.arch & ARCH_64)
	{
		if (check_size(&file, sizeof(struct mach_header_64), F_OFFSET) < 0)
			return (-1);
		mh_64 = (struct mach_header_64*)file.ptr;
		return (handle_mach(file, sw32(mh_64->ncmds, file.arch),
		(struct load_command*)((size_t)file.ptr + sizeof(*mh_64))));
	}
	return (-1);
}

void		print_binary_arch(char *name, cpu_type_t cpu)
{
	ft_printf("%s (for architecture", name);
	if (cpu == CPU_TYPE_X86)
		ft_printf(" i386");
	else if (cpu == CPU_TYPE_POWERPC)
		ft_printf(" ppc");
	ft_printf("):\n");
}

int			handle_fat_64(t_file file, uint32_t nfat_arch)
{
	struct fat_arch_64	*fat;
	int					i;

	if (nfat_arch < 1 || check_size(&file, sizeof(struct fat_arch_64) * nfat_arch, F_OFFSET) < 0)
		return (-1);
	i = nfat_arch;
	fat = (struct fat_arch_64*)((size_t)file.ptr + sizeof(struct fat_header));
	while (i && sw32(fat->cputype, file.arch) != CPU_TYPE_X86_64 && i--)
		fat = (struct fat_arch_64*)((size_t)fat + sizeof(struct fat_arch_64));
	fat = (struct fat_arch_64*)((size_t)file.ptr + sizeof(struct fat_header));
	while (nfat_arch--)
	{
		if (!i)
			print_binary_arch(file.name, sw32(fat->cputype, file.arch));
		file.offset = sw64(fat->offset, file.arch);
		if ((!i && get_arch(file, false) < 0) || ((uint32_t)i == (nfat_arch + 1) && get_arch(file, true) < 0))
			return (-1);
		fat = (struct fat_arch_64*)((size_t)fat + sizeof(struct fat_arch_64));
	}
	return (0);
}

int			handle_fat(t_file file, uint32_t nfat_arch)
{
	struct fat_arch	*fat;
	int				i;

	if (nfat_arch < 1 || check_size(&file, sizeof(struct fat_arch) * nfat_arch, F_OFFSET) < 0)
		return (-1);
	i = nfat_arch;
	fat = (struct fat_arch*)((size_t)file.ptr + sizeof(struct fat_header));
	while (i && sw32(fat->cputype, file.arch) != CPU_TYPE_X86_64 && i--)
		fat = (struct fat_arch*)((size_t)fat + sizeof(struct fat_arch));
	fat = (struct fat_arch*)((size_t)file.ptr + sizeof(struct fat_header));
	while (nfat_arch--)
	{
		if (!i)
			print_binary_arch(file.name, sw32(fat->cputype, file.arch));
		file.offset = sw32(fat->offset, file.arch);
		if ((!i && get_arch(file, false) < 0) || ((uint32_t)i == (nfat_arch + 1) && get_arch(file, true) < 0))
			return (-1);
		fat = (struct fat_arch*)((size_t)fat + sizeof(struct fat_arch));
	}
	return (0);
}

int			handle_fat_header(t_file file)
{
	struct fat_header	*fh;

	if (check_size(&file, sizeof(struct fat_header), F_OFFSET) < 0)
		return (-1);
	fh = (struct fat_header*)file.ptr;
	if (file.arch & ARCH_32)
		return (handle_fat(file, sw32(fh->nfat_arch, file.arch)));
	else if (file.arch & ARCH_64)
		return (handle_fat_64(file, sw32(fh->nfat_arch, file.arch)));
	return (-1);
}

uint8_t		is_valid_arch(uint32_t magic)
{
	return (magic == MH_MAGIC_64 || magic == MH_CIGAM_64
	|| magic == FAT_MAGIC_64 || magic == FAT_CIGAM_64
	|| magic == MH_MAGIC || magic == MH_CIGAM
	|| magic == FAT_MAGIC || magic == FAT_CIGAM ? ARCH_VAL : 0);
}

uint8_t		is_64_arch(uint32_t magic)
{
	return (magic == MH_MAGIC_64 || magic == MH_CIGAM_64
	|| magic == FAT_MAGIC_64 || magic == FAT_CIGAM_64 ? ARCH_64 : ARCH_32);
}

uint8_t		is_cigam_arch(uint32_t magic)
{
	return (magic == MH_CIGAM_64 || magic == MH_CIGAM
	|| magic == FAT_CIGAM || magic == FAT_CIGAM_64 ? CIGAM : MAGIC);
}

uint8_t		is_mach_arch(uint32_t magic)
{
	return (magic == MH_MAGIC_64 || magic == MH_MAGIC
	|| magic == MH_CIGAM || magic == MH_CIGAM_64 ? MACH_O : FAT);
}

int			get_arch(t_file file, bool print_name)
{
	struct mach_header	*mh;

	if (file.offset)
	{
		file.size -= file.offset;
		file.free_size = file.size;
		file.ptr = (char*)((size_t)file.ptr + file.offset);
	}
	if (check_size(&file, sizeof(struct mach_header), F_BEGIN) < 0)
		return (-1);
	mh = (struct mach_header*)file.ptr;
	file.arch = is_valid_arch(mh->magic) | is_mach_arch(mh->magic)
	| is_64_arch(mh->magic) | is_cigam_arch(mh->magic);
	if (print_name && file.arch & MAGIC)
		ft_printf("%s:\n", file.name);
	if (file.arch & ARCH_VAL && file.arch & MACH_O)
		return (handle_mach_header(file, mh));
	else if (file.arch & ARCH_VAL && file.arch & FAT)
		return (handle_fat_header(file));
	else if (!ft_strncmp(file.ptr, ARMAG, SARMAG))
	{
		ft_printf("{red}[NM]{eoc} static library\n");
		return (0);
	}
	return (-2);
}

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
	if ((file->ptr = mmap(0, buf.st_size, PROT_READ, MAP_PRIVATE, fd, 0)) == MAP_FAILED)
		return (-1);
	file->name = ft_strdup(name);
	file->size = buf.st_size;
	file->free_size = buf.st_size;
	file->arch = 0;
	file->offset = 0;
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
		//	if (av[i][j] == OPT_F_MU)
		//		*opts |= OPT_MU;
		//	else
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
	while (j < (opts >> 8))
	{
		if (av[++i][0] == '-')
			continue ;
		file.opts = opts;
		if ((err = init_file(av[i], &file)) < 0
		|| (err = get_arch(file, true)) < 0)
			return (pexit(err == -2 ? "{red}[NM]{eoc} Unknow file format\n"
			: "{red}[NM]{eoc} An error has occurred\n"));
		free(file.name);
		munmap(file.ptr, file.size);
		j++;
	}
}
