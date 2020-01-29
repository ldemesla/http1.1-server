#include "../webserver.hpp"

void	ft_check_conf_extension(const char *file)
{
	int len(ft_strlen((char *)file));

	if (len < 6 ||
			file[len - 5] != '.' ||
			file[len - 4] != 'c' ||
			file[len - 3] != 'o' ||
			file[len - 2] != 'n' ||
			file[len - 1] != 'f')
		error("configuration file: file must have the extension '.conf'", false);
}

void ft_parse_conf(std::vector<t_server> *server, char **av)
{
    int         fd;
    char        *line;
    std::string line_string;
	std::string path_temp;

	if (av[1] != NULL)
	{
		if ((fd = open(av[1], O_RDONLY)) <= 0)
	       	error("open() #2", true);
	}
	else
	{
		path_temp += ft_cut_path(av[0]);
		path_temp += '/';
		path_temp += DEFAULT_CONF_PATH;
		if ((fd = open(path_temp.c_str(), O_RDONLY)) <= 0)
        	error("open() #2", true);
	}
    while (get_next_line(fd, &line))
    {
        line_string = line;
        ft_free(line);
        if (!line_string.compare("server {"))
            server->push_back(ft_add_server(fd, line_string));
    }
    if (server->empty())
        error("parsing: no server has been specified", false);
    ft_free(line);
    close(fd);
}