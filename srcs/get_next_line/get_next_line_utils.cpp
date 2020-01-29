/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line_utils.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mabois <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/10/21 18:37:02 by mabois            #+#    #+#             */
/*   Updated: 2019/10/24 14:29:14 by mabois           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../webserver.hpp"

void	ft_init_line_buffer_lu(char ***line, char *buffer, int *lu)
{
	*lu = 0;
	buffer[BUFFER_SIZE_GNL] = '\0';
	**line = (char *)malloc(sizeof(char) * 1);
	line[0][0][0] = '\0';
}

int		ft_fd_manager(int fd, int *i, int *ret, int *last_fd)
{
	if (*last_fd == 0)
		*last_fd = fd;
	if (*last_fd != fd)
	{
		*i = BUFFER_SIZE_GNL;
		*ret = BUFFER_SIZE_GNL;
		*last_fd = fd;
		return (1);
	}
	return (0);
}

void	ft_add_char(char **line, char c)
{
	char	*new_line;
	int		i;
	int		len;

	if (*line == NULL)
	{
		new_line = (char *)malloc(sizeof(char) * 2);
		new_line[0] = c;
		new_line[1] = '\0';
	}
	else
	{
		len = ft_strlen(*line);
		new_line = (char *)malloc(sizeof(char) * (len + 2));
		i = 0;
		while (i < len)
		{
			new_line[i] = line[0][i];
			i++;
		}
		new_line[len] = c;
		new_line[len + 1] = '\0';
	}
	free(*line);
	*line = new_line;
}
