#include "../webserver.hpp"

void load_error_page(t_client &client)
{
    char buffer[BUFFER_SIZE];
    std::string error_page;
    struct stat info;

    getcwd(buffer, BUFFER_SIZE);
    chdir(client.server->root.c_str());
    client.read_fd = open(client.server->error_page.c_str(), O_RDONLY);
    if (client.read_fd == -1)
    {
        error_page = generate_error_page(404);
        client.request.res = "HTTP/1.1 404 Not Found\r\nContent-Length: " + ft_int_to_string(error_page.size()) + "\r\n\r\n"+ error_page;
        client.read_fd = 0;
    }
    else
    {
        stat(client.server->error_page.c_str(), &info);
        client.request.res = "HTTP/1.1 404 Not Found\r\nContent-Length: "+ ft_long_to_string(info.st_size) + "\r\n\r\n";
    }
    chdir(buffer);
}

std::string	ft_cut_path(char *path)
{
	int i(ft_strlen(path));
	std::string cuted_path;

	while (--i >= 0)
		if (path[i] == '/')
			break;
	if (path[i] == '/')
	{
		int j(0);
		while (j < i)
			cuted_path += path[j++];
	}
	return (cuted_path);
}

bool	ft_path_with_slash(char *path)
{
	return (path != NULL && path[0] == '/');
}


bool is_end(t_client client)
{
	std::map<std::string, std::string>::iterator it;
	std::map<std::string, std::string>::iterator it2;
	std::map<std::string, std::string>::iterator it3;
	std::map<std::string, std::string>::iterator end;

	it = client.request.headers.find("Transfer-Encoding");
	it2 = client.request.headers.find("Content-Encoding");
	it3 = client.request.headers.find("Content-Length");
	end = client.request.headers.end();
	if (it != end && (!it->second.compare("chunked") || !it->second.compare("chunked, gzip") || !it->second.compare("gzip, chunked")) && !client.request.pt_data.end)
		return (false);
	if (it != end && (!it->second.compare("gzip") || (it2 != client.request.headers.end() && (!it2->second.compare("gzip") && ((int)client.request.pt_data.compress_req.size() < client.request.pt_data.size)))))
		return (false);
	else if (it != end && client.request.pt_data.size > client.request.bytes_read)
		return (false);
	return (true);
}

std::string	ft_get_initial_path()
{
	std::string		initial_path;
	char			buffer[GETCWD_BUFFER_SIZE + 1];
	
	if (getcwd(buffer, GETCWD_BUFFER_SIZE) == NULL)
		error("getcwd", true);
	initial_path = buffer;
	return (initial_path);
}
