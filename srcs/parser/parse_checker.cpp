#include "../webserver.hpp"

void	ft_check_server_parsing(t_server &server)
{
	if (server.port == -1)
		error("check server parsing: server port not specified", false);
}

void	ft_check_location_parsing(t_location &location)
{
	location.root.size(); /* inutile */
	return ;
}
