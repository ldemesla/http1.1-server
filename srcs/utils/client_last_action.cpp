#include "../webserver.hpp"

void	ft_reset_last_action_client(t_client &client)
{
	gettimeofday(&client.last_action, NULL);
}

unsigned long 	ft_get_last_action_client(t_client &client)
{
    struct timeval  now;

    gettimeofday(&now, NULL);
	return (now.tv_sec - client.last_action.tv_sec);
}

int    ft_timeout_clients(std::vector<t_client>::iterator it, std::vector<t_client> &clients, fd_set &ini_set_read, fd_set &ini_set_write)
{
	int	nbr_client_timeout(0);
	
	while (it != clients.end())
    {
        if ((unsigned long)ft_get_last_action_client(*it) > (unsigned long)TIME_OUT_CLIENT_SEC)
        {
            ft_disconnect_client(it, clients, ini_set_read, ini_set_write);
            it = clients.begin();
			nbr_client_timeout++;
            continue ;
        }
        it++;
    }
	return (nbr_client_timeout);
}