#include "../webserver.hpp"

void	ft_sigpipe_signal_catcher(int n)
{
	(void)n;
	return ;
}

void	ft_sigint_signal_catcher(int n)
{
	std::cout << "\b\bshutdown servers (0/3): start: signal[" << n << "]" << std::endl;
	std::string path = (g_cleaner.servers->back().root + "/.temp");
	
	std::cout << g_cleaner.servers->back().root << std::endl;
	rmdir(path.c_str());
	/* ====================================== fd_set */
	if (g_cleaner.ini_set_read != NULL)
		FD_ZERO(g_cleaner.ini_set_read);
	else
		std::cout << "shutdown servers (1/3) error: g_cleaner.ini_set_read == NULL..." << std::endl;
	if (g_cleaner.ini_set_write != NULL)
		FD_ZERO(g_cleaner.ini_set_write);
	else
		std::cout << "shutdown servers (1/3) error: g_cleaner.ini_set_write == NULL..." << std::endl;
	std::cout << "shutdown servers (1/3): all fd_set reset" << std::endl;

	/* ====================================== fd_clients */
	if (g_cleaner.clients != NULL)
		for (std::vector<t_client>::iterator it(g_cleaner.clients->begin()); it != g_cleaner.clients->end(); it++)
		{
			close(it->fd);
			if (it->ssl_fd != NULL)
			{	
				SSL_shutdown(it->ssl_fd);
				SSL_free(it->ssl_fd);
				it->ssl_fd = NULL;
			}
			if (it->read_fd != 0)
				close(it->read_fd);
		}
	else
		std::cout << "shutdown servers (2/3) error: g_cleaner.clients == NULL..." << std::endl;
	std::cout << "shutdown servers (2/3): all clients fd closed" << std::endl;
	
	/* ====================================== fd_servers */
	if (g_cleaner.servers != NULL)
		for (std::vector<t_server>::iterator it(g_cleaner.servers->begin()); it != g_cleaner.servers->end(); it++)
			if (it->ssl_fd != NULL)
			{
				SSL_shutdown(it->ssl_fd);
				SSL_free(it->ssl_fd);
				it->ssl_fd = NULL;
			}
			else
				close(it->fd);
	else
		std::cout << "shutdown servers (3/3) error: g_cleaner.servers == NULL..." << std::endl;
	std::cout << "shutdown servers (3/3): all servers fd closed" << std::endl;
	exit(EXIT_SUCCESS);
	return ;
}
