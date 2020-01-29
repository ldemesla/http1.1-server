/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mabois <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/10/13 05:29:32 by mabois            #+#    #+#             */
/*   Updated: 2019/10/13 05:29:33 by mabois           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../webserver.hpp"

int		get_next_line(int fd, char **line)
{
	static t_st	sta = {.i = BUFFER_SIZE_GNL, .ret = BUFFER_SIZE_GNL, .last_fd = 0};

	if (fd < 0 || line == NULL)
		return (-1);
	ft_init_line_buffer_lu(&line, sta.buffer, &sta.lu);
	ft_fd_manager(fd, &sta.i, &sta.ret, &sta.last_fd);
	while (sta.i <= sta.ret && sta.buffer[sta.i] != '\n')
		if (sta.i == sta.ret && (sta.i = 0) == 0)
		{
			if ((sta.ret = read(fd, sta.buffer, BUFFER_SIZE_GNL)) < 0)
				return (-1);
			else if (sta.ret == 0)
			{
				sta.i = BUFFER_SIZE_GNL;
				sta.ret = BUFFER_SIZE_GNL;
				return (sta.lu);
			}
			sta.lu = (sta.ret == BUFFER_SIZE_GNL) ? 1 : 0;
			continue;
		}
		else
			ft_add_char(line, sta.buffer[sta.i++]);
	if (++sta.i <= sta.ret)
		return (1);
	return (1);
}
