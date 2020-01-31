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

bool get_location(t_client &client)
{
	std::vector<t_location>::iterator	it;
	std::vector<t_location>::iterator	ending;
	std::string							route;
	std::string							path;
	int i;

    route = get_route(client.request);
	path = route;
	i = route.size() - 1;
	it = client.server->location.begin();
	ending = client.server->location.end();
	while (it != ending)
	{
       	if ((*it).location.compare(route) == 0)
		{
			client.request.loc = *it;
			client.request.file.clear();
			return true;
		}
		it++;
	}
	do
	{
		while (route[i] != '/' && i != 0)
			i--;
		route = route.substr(0, i);
		it = client.server->location.begin();
		ending = client.server->location.end();
		while (it != ending)
		{
			if (!it->location.compare(route))
			{
				client.request.loc = *it;
				client.request.file = path.substr(it->location.size(), path.size() - it->location.size());
				return true;
			}
			it++;
		}
	} while (route.size() > 0);
	load_error_page(client);
	return (false);
}