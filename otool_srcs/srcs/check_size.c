/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   check_size.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: clanier <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/12/17 21:03:24 by clanier           #+#    #+#             */
/*   Updated: 2017/12/17 21:03:35 by clanier          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "otool.h"

int			check_size(t_file *file, long int size, uint8_t flag)
{
	if (flag & F_BEGIN && file->size - size < 0)
		return (-1);
	if (flag & F_OFFSET && (file->free_size -= size) < 0)
		return (-1);
	return (0);
}
