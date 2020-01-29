#include "../webserver.hpp"

/* root | methods | listing | save | php | index */

void    ft_set_location_root(t_location &server, std::string &line_string)
{
	if (!(server.root.empty()))
		error("set server root: root is already specified", false);
	server.root = ft_get_val(line_string);
}

void    ft_set_location_methods(t_location &server, std::string &line_string)
{
	if (!(server.methods.empty()))
		error("set server methods: methods is already specified", false);
	server.methods = ft_get_methods(line_string);
}

void    ft_set_location_listing(t_location &server, std::string &line_string)
{
	if (!(server.listing.empty()))
		error("set server listing: listing is already specified", false);
	server.listing = ft_get_val(line_string);
}

void    ft_set_location_save(t_location &server, std::string &line_string)
{
	if (!(server.save.empty()))
		error("set server save: save is already specified", false);
	server.save = ft_get_val(line_string);
}

void    ft_set_location_php(t_location &server, std::string &line_string)
{
	if (!(server.php.empty()))
		error("set server php: php is already specified", false);
	server.php = ft_get_val(line_string);
}

void    ft_set_location_index(t_location &server, std::string &line_string)
{
	if (!(server.index.empty()))
		error("set server index: index is already specified", false);
	server.index = ft_get_index(line_string);
}
