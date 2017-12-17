/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   swap.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: clanier <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/12/17 20:05:00 by clanier           #+#    #+#             */
/*   Updated: 2017/12/17 20:05:52 by clanier          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "nm.h"

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
