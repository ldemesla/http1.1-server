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