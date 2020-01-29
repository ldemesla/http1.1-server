#include "../webserver.hpp"

/* port | error_page | host | compression | TLS | body_size | location */

void    ft_set_server_port(t_server &server, std::string &line_string)
{
	if (server.port != -1)
		error("set server port: port is already specified", false);
    if ((server.port = ft_get_int(line_string)) < 0)
		error("set server port: port cannot be negative", false);
	else if (server.port > MAX_PORT)
		error("set server port: port cannot be greater than " + ft_int_to_string(server.port), false);
}

void    ft_set_server_error_page(t_server &server, std::string &line_string)
{
	if (!(server.error_page.empty()))
		error("set server error_page: error_page is already specified", false);
    server.error_page = ft_get_val(line_string);
}

void    ft_set_server_host(t_server &server, std::string &line_string)
{
	if (!(server.host.empty()))
		error("set server host: host is already specified", false);
    server.host = ft_get_val(line_string);
}

void    ft_set_server_compression(t_server &server, std::string &line_string)
{
	if (!(server.compression.empty()))
		error("set server compression: compression is already specified", false);
    server.compression = ft_get_val(line_string);
}

void    ft_set_server_TLS(t_server &server, std::string &line_string)
{
	if (!(server.TLS.empty()))
		error("set server TLS: TLS is already specified", false);
    server.TLS = ft_get_val(line_string);
}

void    ft_set_server_body_size(t_server &server, std::string &line_string)
{
	if (server.body_size != -1)
		error("set server body_size: body_size is already specified", false);
    if ((server.body_size = ft_get_int(line_string)) < 0)
		error("set server body_size: body_size cannot be negative", false);
}

void    ft_set_server_cert(t_server &server, std::string &line_string)
{
	if (!(server.cert_path.empty()))
		error("set server cert: cert is already specified", false);
    server.cert_path = ft_get_val(line_string);
}

void    ft_set_server_key(t_server &server, std::string &line_string)
{
	if (!(server.key_path.empty()))
		error("set server cert: cert is already specified", false);
    server.key_path = ft_get_val(line_string);
}
