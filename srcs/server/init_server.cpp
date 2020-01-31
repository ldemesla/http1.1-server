#include "../webserver.hpp"

struct timeval	ft_init_timeout_select(time_t sec, time_t u_sec)
{
	struct timeval	timout_select;

	timout_select.tv_sec = sec;
	timout_select.tv_usec = u_sec;
	return (timout_select);
}

void	ft_cut_conf_path(std::string &path)
{
	std::string new_path;
	int	i(path.size());
	char buffer[GETCWD_BUFFER_SIZE + 1];

	while (--i > 0)
		if (path[i] == '/')
			break;
	new_path = getcwd(buffer, GETCWD_BUFFER_SIZE);
	new_path += "/";
	path = path.substr(0, i);
	new_path += path;
	path = new_path;
}

void	ft_init_fd_set(std::vector<t_server> &server, fd_set &ini_set_read, fd_set &ini_set_write)
{
	FD_ZERO(&ini_set_read);
	FD_ZERO(&ini_set_write);

	for (std::vector<t_server>::iterator it(server.begin()); it != server.end(); it++)			/* new server */
		FD_SET(it->fd, &ini_set_read);
}

void ft_init_server(std::vector<t_server> *server, char **av)
{
	int un;
	std::vector<t_server>::iterator end(server->end());
	bool	openssl_is_init(false);

	for (std::vector<t_server>::iterator it(server->begin()); it != end; it++)
	{
		if (chdir(g_initial_path.c_str()) < 0)
			error("chdir", true);

		if (av[1] == NULL)
		{
			it->root += ft_cut_path(av[0]);
			it->root += '/';
			it->root += DEFAULT_CONF_PATH;
		}
		else
			it->root = av[1];
		
		if (!ft_path_with_slash((char *)it->root.c_str()))
			ft_cut_conf_path(it->root);
		else
			it->root = ft_cut_path((char *)it->root.c_str());
		
		chdir(it->root.c_str());
		if ((it->fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
			error("socket()", true);
		if (setsockopt(it->fd,  SOL_SOCKET, SO_REUSEADDR, &un, sizeof(int)) < 0)
			error("setsockopt()", true);
		it->addr.sin_family = AF_INET;
    	it->addr.sin_addr.s_addr = htonl(INADDR_ANY);
    	it->addr.sin_port = htons(it->port);
		if (bind(it->fd, (struct sockaddr*)&it->addr, sizeof(it->addr)) == -1)
			error("bind()", true);
		if (listen(it->fd, LISTEN_MAX) == -1)
			error("listen()", true);
		it->ctx = NULL;															/* tls start*/
		if (!it->TLS.compare("on"))
		{
			if (!openssl_is_init)
			{
				init_openssl();
				openssl_is_init = true;
			}
    		it->ctx = create_context();
    		configure_context(it->ctx, *it);
			SSL_CTX_set_verify(it->ctx, SSL_VERIFY_NONE, krx_ssl_verify_peer);
		}
	}
}
