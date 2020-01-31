#include "../webserver.hpp"

t_cleaner	g_cleaner;

int		main(int ac, char **av)
{
	std::vector<t_server> servers;

	signal(SIGINT, ft_sigint_signal_catcher);
	signal(SIGPIPE, ft_sigpipe_signal_catcher);
	if (ac > 2)
		error("expected one argument", false);
	ft_parse_conf(&servers, av);
	ft_init_server(&servers, av);
	ft_server(servers);

	while (1);
}
