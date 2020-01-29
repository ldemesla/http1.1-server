#include "../webserver.hpp"

void ft_listing(t_client &client)
{
	int			fd;
	struct stat	info;
	std::string path;
	std::string final_body;

	if (client.request.file.empty() == true)
		client.request.file += ".";
	final_body = ft_generate_listing_page(client);
	if (!client.server->compression.compare("on"))
		path = client.temp_file + "_listing";
	else
		path = client.temp_file;
    fd = open_temp_file(client, path, O_RDWR | O_CREAT | O_TRUNC, 1);
	write(fd, final_body.c_str(), final_body.size());
	if (!client.server->compression.compare("on"))
		compressed_file_fd(client, path);
	else
	{
		stat_temp(client, &info, path);
    	client.request.file_size = info.st_size;
		client.read_fd = open_temp_file(client, path, O_RDONLY, 0);
		client.request.res = generate_200_header(0, client.request.file, client.request.file_size);
	}
	close(fd);
}