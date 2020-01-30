#include "../webserver.hpp"

std::string ft_long_to_string(unsigned long nbr)
{
	char            c;
	std::string     temp;
	std::string     result;
	do
	{
		c = nbr % 10;
		temp = c + 48;
		result = temp + result;
		nbr /= 10;
	} while (nbr > 0);
	return (result);
}

std::string get_host_header(std::string request)
{
	int i;
	int size;
	int k;
	std::string host;

	i = 0;
	size = request.size();
	while (i < size)
	{
		if (i >= 3 && request[i - 3] == '\r' && request[i - 2] == '\n' &&
				request[i - 1] == '\r' && request[i] == '\n')
			break ;
		if (i > 2 && request[i - 2] == '\r' && request[i - 1] == '\n')
		{
			if (!request.compare(i, 5, "Host:"))
			{
				k = i;
				while (request[k] != '\n' && request[k] != '\r')
					k++;
				host = request.substr(i + 6, k - (i + 6));
			}
		}
		i++;
	}
	return (host);
}

std::string generate_error_page(int code)
{
	std::string error;

	error = "<!DOCTYPE html>\n<html>\n<head>\n<title>Webserver</title>\n<style>\
    \tbody {\t\twidth: 35em;\n\t\tmargin: 0 auto;\n\t\tfont-family: Tahoma, Verdana, Arial, sans-serif;\
	\n\t}\n</style>\n</head>\n<body>\n\t<h1> This is a " + ft_int_to_string(code) + " error</h1>\n\
	</body>\n</html>";
	return (error);
}

int	ft_send(t_client &client)
{
	int ret(-1);

	ft_reset_last_action_client(client);
	if (client.ssl_fd != NULL)
		ret = SSL_write(client.ssl_fd , client.request.res.c_str(), client.request.res.size());
	else
	{
		std::string a = (char*)client.request.res.c_str();
		ret = send(client.fd, client.request.res.c_str(), client.request.res.size(), 0);
	}
	if (ret >= 0)
	{
		if (ret != (int)client.request.res.size())
			client.request.res = client.request.res.substr(ret, client.request.res.size() - ret);
		else
			client.request.res.clear();
	}
	return (ret);
}

std::string set_header(t_client &client, std::string file)
{
	std::string new_file;
	int fd;
	int fd2;
	int ret;
	int i = 0;
	int brk = 0;
	std::string res;
	char buffer[BUFFER_SIZE];

	if (!client.server->compression.compare("on"))
		new_file = client.temp_file + "_gzip";
	else
		new_file = client.temp_file;
	fd = open_temp_file(client, file, O_RDONLY, 0);
	fd2 = open_temp_file(client, new_file, O_RDWR | O_CREAT | O_TRUNC, 1);
	while (1)
	{
		ret = read(fd, buffer, BUFFER_SIZE);
		res.append(buffer, ret);
		while (res[i])
		{
			if (i >= 3 && res[i - 3] == '\r' && res[i - 2] == '\n' &&
			res[i - 1] == '\r' && res[i] == '\n')
			{
				i++;
				brk = 1;
				break ;
			}
			i++;
		}
		if (ret < BUFFER_SIZE || brk)
			break ;
	}
	if (!brk)
	{
		close(fd);
		fd = open_temp_file(client, file, O_RDONLY, 0);
		ret = read(fd, buffer, BUFFER_SIZE);
		write(fd2, buffer, ret);
	}
	else
	{
		while (res[i])
			write(fd2, &buffer[i++], 1);
		while (1)
		{
			ret = read(fd, buffer, BUFFER_SIZE);
			write(fd2, buffer, ret);
			if (ret < BUFFER_SIZE)
				break ;
		}
	}
	close(fd);
	close(fd2);
	delete_temp_file(client, file);
	return (new_file);
}

int check_php_extension(std::string file)
{
	int size;

	size = file.size() - 1;
	if (size > 3 && file[size] == 'p' && file[size - 1] == 'h' && file[size - 2] == 'p' && file[size -3] == '.')
		return (1);
	return (0);
}