#include "../webserver.hpp"

void ft_disconnect_client(std::vector<t_client>::iterator &it, std::vector<t_client> &clients, fd_set &ini_set_read, fd_set &ini_set_write)
{
	close(it->fd);
	FD_CLR(it->fd, &ini_set_read);
	FD_CLR(it->fd, &ini_set_write);
	delete_temp_file(*it, it->temp_file);
	if (it->ssl_fd != NULL)
	{
		SSL_shutdown(it->ssl_fd);
		SSL_free(it->ssl_fd);
		it->ssl_fd = NULL;
	}
	if (it->read_fd != 0)
		close(it->read_fd);
	clients.erase(it);
}

void ft_accept_disconnect(t_server &server)
{
	struct sockaddr_in client_addr;
	socklen_t len;
	int fd;

	fd = accept(server.fd, (struct sockaddr *)&client_addr, &len);
	close(fd);
}

int ft_max_fd(std::vector<t_server> &server, std::vector<t_client> &clients)
{
	int max(0);

	for (std::vector<t_server>::iterator it(server.begin()); it != server.end(); it++)
		if (it->fd > max)
			max = it->fd;
	for (std::vector<t_client>::iterator it(clients.begin()); it != clients.end(); it++)
		if (it->fd > max)
			max = it->fd;
	return (max + 1);
}

int ft_read(t_client &client, char *buffer)
{
	int ret(0);

	if (client.ssl_fd == NULL)
		ret = recv(client.fd, buffer, SERVER_BUFFER_SIZE, 0);
	else
	{
		while ((ret = SSL_read(client.ssl_fd, buffer, SERVER_BUFFER_SIZE)) <= 0)
		{
			int ret_get_error(SSL_get_error(client.ssl_fd, ret));
			if (ret_get_error == SSL_ERROR_WANT_READ || ret_get_error == SSL_ERROR_WANT_WRITE)
				continue;
			else
				break;
		}
	}
	return (ret);
}

void ft_accept(t_server &server, t_client &client_connection_temp, std::vector<t_client> &clients, fd_set &ini_set_read, fd_set &ini_set_write)
{
	struct sockaddr_in client_addr;
	socklen_t len;
	int ret;

	len = sizeof(client_addr);
	if ((client_connection_temp.fd = accept(server.fd, (struct sockaddr *)&client_addr, &len)) <= 0)
		error("accept", true);
	client_connection_temp.server = &server;
	client_connection_temp.ssl_fd = NULL;
	client_connection_temp.read_fd = 0;
	ft_reset_last_action_client(client_connection_temp);
	if (!server.TLS.compare("on") && client_connection_temp.ssl_fd == NULL) /* tls start */
	{
		client_connection_temp.ssl_fd = SSL_new(server.ctx);
		if (SSL_set_fd(client_connection_temp.ssl_fd, client_connection_temp.fd) <= 0)
			std::cerr << "SSL_set_fd return <= 0" << std::endl;
		while ((ret = SSL_accept(client_connection_temp.ssl_fd)) <= 0)
		{
			int ret_get_error;
			ret_get_error = SSL_get_error(client_connection_temp.ssl_fd, ret);
			if (ret_get_error == SSL_ERROR_WANT_READ || ret_get_error == SSL_ERROR_WANT_WRITE)
				continue;
			else
			{
				close(client_connection_temp.fd);
				FD_CLR(client_connection_temp.fd, &ini_set_read);
				FD_CLR(client_connection_temp.fd, &ini_set_write);
				SSL_shutdown(client_connection_temp.ssl_fd);
				SSL_free(client_connection_temp.ssl_fd);
				return;
			}
		}
	}
	client_connection_temp.temp_file = client_connection_temp.server->host + ":" + ft_int_to_string(client_connection_temp.server->port) + ":" + ft_int_to_string(client_connection_temp.fd);
	clients.push_back(client_connection_temp);

	FD_SET(client_connection_temp.fd, &ini_set_read);
}

void ft_recv(std::vector<t_client>::iterator &it, char *buffer, std::vector<t_client> &clients, fd_set &ini_set_read, fd_set &ini_set_write)
{
	std::string error;
	int ret;

	ft_reset_last_action_client(*it);
	if ((ret = ft_read(*it, buffer)) <= 0)
	{
		ft_disconnect_client(it, clients, ini_set_read, ini_set_write);
		return;
	}
	if (ft_save_request(buffer, it, ret))
		FD_SET(it->fd, &ini_set_write);
}

int ft_number_of_fd_used(std::vector<t_server> &server, std::vector<t_client> &clients)
{
	int fd_nbr(0);

	for (std::vector<t_server>::iterator it(server.begin()); it != server.end(); it++)
		fd_nbr++;
	for (std::vector<t_client>::iterator it(clients.begin()); it != clients.end(); it++)
		it->read_fd == 0 ? fd_nbr++ : fd_nbr += 2;
	return (fd_nbr + INITAL_USED_FD);
}

bool need_to_disconnect(std::vector<t_client> &clients, fd_set &set, fd_set &ini_set_read, fd_set &ini_set_write)
{
	bool ret(false);

	std::vector<t_client>::iterator it = clients.begin();
	std::vector<t_client>::iterator end = clients.end();

	while (it != end)
	{

		if (it->disconnect == 1 && FD_ISSET(it->fd, &set) && it->read_fd == 0)
		{
			ft_send(*it);
			it->disconnect = 0;
			delete_temp_file(*it, it->temp_file);
			it->request.request.clear();
			ft_disconnect_client(it, clients, ini_set_read, ini_set_write);
			it = clients.begin();
			ret = true;
			continue;
		}
		it++;
	}
	return (ret);
}

void ft_server(std::vector<t_server> &servers)
{
	int ret;
	char buffer[SERVER_BUFFER_SIZE + 1];
	char read_buffer[MAX_SIZE + 1];
	std::string temp_path;
	std::string temp_paht_2;

	fd_set ini_set_read;
	fd_set ret_set_read;
	fd_set ini_set_write;
	fd_set ret_set_write;

	t_client client_connection_temp;
	std::vector<t_client> clients;

	unsigned int force_timeout_client(0);
	struct timeval time_out_select;

	g_cleaner.clients = &clients;
	g_cleaner.servers = &servers;
	g_cleaner.ini_set_read = &ini_set_read;
	g_cleaner.ini_set_write = &ini_set_write;
	ft_init_fd_set(servers, ini_set_read, ini_set_write);
	while (1)
	{
		ret_set_read = ini_set_read;
		ret_set_write = ini_set_write;
		time_out_select = ft_init_timeout_select(TIME_OUT_SELECT_SEC, TIME_OUT_SELECT_USEC);
		ret = select(ft_max_fd(servers, clients), &ret_set_read, &ret_set_write, NULL, &time_out_select); /* SELECT */
		if (ret == 0 || force_timeout_client++ >= FORCE_TIME_OUT_CLIENT)
		{
			ft_timeout_clients(clients.begin(), clients, ini_set_read, ini_set_write);
			force_timeout_client = 0;
			continue;
		}
		for (std::vector<t_client>::iterator it(clients.begin()); it != clients.end(); it++) /* old client */
		{
			if (FD_ISSET(it->fd, &ret_set_write)) /* WRITE / CLOSE */
			{
				temp_paht_2 = it->server->root;
				chdir(it->request.loc.root.c_str());
				if (it->request.pt_data.on)
				{
					if (!ft_request_handling(*it))
						FD_CLR(it->fd, &ini_set_write);
				}
				else if (!it->request.res.empty())
				{
					if (ft_send(*it) == -1 || (it->disconnect && !it->read_fd))
					{
						delete_temp_file(*it, it->temp_file);
						it->request.request.clear();
						ft_disconnect_client(it, clients, ini_set_read, ini_set_write);
					}
				}
				else if (it->read_fd != 0)
				{
					ret = read(it->read_fd, read_buffer, MAX_SIZE);
					if (ret < MAX_SIZE)
					{
						delete_temp_file(*it, it->temp_file);
						it->request.request.clear();
						FD_CLR(it->fd, &ini_set_write);
						close(it->read_fd);
						it->read_fd = 0;
						it->request.pt_data.on = 0;
					}
					it->request.res.append(read_buffer, ret);
					ft_send(*it);
				}
				else if (!ft_request_handling(*it))
				{
					if (!it->request.pt_data.on)
						it->request.request.clear();
					FD_CLR(it->fd, &ini_set_write);
				}
				chdir(temp_paht_2.c_str());
				break;
			}
			else if (FD_ISSET(it->fd, &ret_set_read)) /* RECV */
			{
				ft_recv(it, buffer, clients, ini_set_read, ini_set_write);
				break;
			}
		}
		for (std::vector<t_server>::iterator it(servers.begin()); it != servers.end(); it++) /* new client */
			if (FD_ISSET(it->fd, &ret_set_read))											 /* ACCECPT */
			{
				if (ft_number_of_fd_used(servers, clients) >= FD_LIMIT_ACCEPTED)
					ft_accept_disconnect(*it);
				else
					ft_accept(*it, client_connection_temp, clients, ini_set_read, ini_set_write);
				break;
			}
	}
}
