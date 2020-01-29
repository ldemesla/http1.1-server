#include "../webserver.hpp"

void        ft_check_semicolon(std::string &line_string)
{
    if (line_string[line_string.size() - 1] != ';' &&
            line_string.substr(0, 9).compare("\tlocation") != 0)
	{
		std::string error_str("parsing: line does not end with a semicolon, line:\n|" + line_string + "|");
        error(error_str.c_str(), false);
	}
}

bool		ft_check_comment(std::string &line_string)
{
	for (unsigned long i(0); i < line_string.size(); i++)
		if (line_string[i] == '#')
			return (true);
		else if (line_string[i] != '\t' && line_string[i] != ' ')
			break ;
	return (false);
}

bool		ft_check_empty_line(std::string &line_string)
{
	for (unsigned long i(0); i < line_string.size(); i++)
		if (line_string[i] != ' ' && line_string[i] != '\t')
			return (false);
	return (true);
}

t_location	ft_add_location(int file, std::string line_string)
{
    if (line_string[line_string.size() - 1] == ';')
        error("parsing: line cannot end with semicolon", false);
    char    *line(NULL);
    t_location location;
    int i = 10;
    std::map<std::string, location_set_function>  caller;
    while (line_string[i] && line_string[i] != ';')
        i++;
    location.location = line_string.substr(11, i - 11);
    get_next_line(file, &line);
    ft_free(line);
    caller["\t\troot"] =	&ft_set_location_root;
    caller["\t\tmethods"] =	&ft_set_location_methods;
    caller["\t\tlisting"] =	&ft_set_location_listing;
    caller["\t\tsave"] =	&ft_set_location_save;
    caller["\t\tphp"] =		&ft_set_location_php;
    caller["\t\tindex"] =	&ft_set_location_index;
    while (get_next_line(file, &line) && (line_string = line).compare("\t}"))
    {
        ft_free(line);
		if (ft_check_empty_line(line_string) || ft_check_comment(line_string))
			continue ;
        ft_check_semicolon(line_string);
        if (caller.count(ft_cut_line_string(line_string)) == 1)
            caller[ft_cut_line_string(line_string)](location, line_string);
        else
            error("parsing: " + line_string + " is not a valide feature of location", false);
    }
    ft_free(line);
	ft_check_location_parsing(location);
    return (location);
}

t_server	ft_add_server(int file, std::string line_string)
{
    char        *line(NULL);
    t_server    server;
    server.port = -1;
    server.body_size = -1;
	server.ssl_fd = NULL;
    std::map<std::string, server_set_function>    caller;
    caller["\tport"] =			&ft_set_server_port;
    caller["\terror_page"] =	&ft_set_server_error_page;
    caller["\thost"] =			&ft_set_server_host;
    caller["\tcompression"] =	&ft_set_server_compression;
    caller["\tTLS"] =			&ft_set_server_TLS;
    caller["\tbody-size"] =		&ft_set_server_body_size;
	caller["\tcert"] =			&ft_set_server_cert;
	caller["\tkey"] =			&ft_set_server_key;
    while (get_next_line(file, &line) && (line_string = line).compare("}"))
    {
        ft_free(line);
		if (ft_check_empty_line(line_string) || ft_check_comment(line_string))
			continue ;
        ft_check_semicolon(line_string);
        if (line_string.compare(0, 11, "\tlocation /") == 0)
            server.location.push_back(ft_add_location(file, line_string));
        else if (caller.count(ft_cut_line_string(line_string)) == 1)
            caller[ft_cut_line_string(line_string)](server, line_string);
        else
            error("parsing: " + line_string + " is not a valide feature of server", false);
    }
    ft_free(line);
	ft_check_server_parsing(server);
    return (server);
}
