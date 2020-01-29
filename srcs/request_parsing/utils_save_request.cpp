#include "../webserver.hpp"

std::string		get_route(t_request &request)
{
	int i;
    std::string route;

    i = request.method.size() + 2;
	while (request.request[i] != ' ' && request.request[i] != '?')
		i++;
	route = request.request.substr(request.method.size() + 2 , i - request.method.size() - 2);
	return (route);
}

std::string generate_304_header(int gzip, std::string file)
{
	std::string features;

	features = ft_get_features(file, 1);
    std::string header("HTTP/1.1 304 Not Modified\r\nContent-Language: en-US\r\nDate: " + get_date() +"\r\nServer: Webserver/1.0\
	\r\nEtag: \"" + ft_sha256(features) +"\"\r\nContent-Type: " +  get_content_type(file) + "\r\n");
	if (gzip)
        header += "Content-Encoding: gzip\r\n";
	header += "\r\n";
    return (header);
}

void get_location(t_server *server, t_request &request)
{
	std::vector<t_location>::iterator	it;
	std::vector<t_location>::iterator	ending;
	std::string							route;
	std::string							path;
	int i;

    route = get_route(request);
	path = route;
	i = route.size() - 1;
	it = server->location.begin();
	ending = server->location.end();
	while (it != ending)
	{
       	if ((*it).location.compare(route) == 0)
		{
			request.loc = *it;
			request.file.clear();
			return ;
		}
		it++;
	}
	do
	{
		while (route[i] != '/' && i != 0)
			i--;
		route = route.substr(0, i);
		it = server->location.begin();
		ending = server->location.end();
		while (it != ending)
		{
			if (!it->location.compare(route))
			{
				request.loc = *it;
				request.file = path.substr(it->location.size(), path.size() - it->location.size());
				return ;
			}
			it++;
		}
	} while (route.size() > 0);
}