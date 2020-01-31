# include "../webserver.hpp"

bool					ft_options(t_client &client)
{
	client.request.res = "HTTP/1.1 200 ok\r\nAllow: ";
	std::vector<std::string>::iterator it;
	std::vector<std::string>::iterator end;

	it = client.request.loc.methods.begin();
	end = client.request.loc.methods.end();
	end--;
	while (it != end)
	{
		client.request.res += *it + ", ";
		it++;
	}
	client.request.res += *it;
	client.request.res+= "\r\n";
	client.request.res += "Date: " + get_date() + "\r\nServer: Webserver/1.0\r\nContent-Length: 0\r\n\r\n";
	ft_send(client);
	return (false);
}

bool					ft_delete(t_client &client)
{
	unlink(client.request.file.c_str());
	client.request.res = "HTTP/1.1 204 No Content\r\nDate: " + get_date() + "\r\nServer: Webserver/1.0\r\n\r\n";
	ft_send(client);
	return (false);
}

bool					ft_get(t_client &client)
{
	char buffer[MAX_SIZE + 1];
	int ret;
	struct stat info;

	stat(client.request.file.c_str(), &info);
	if (info.st_mode & S_IFDIR || !client.request.file.compare(""))
		ft_listing(client);
	else if (client.request.loc.php.compare("off") && check_php_extension(client.request.file))
		ft_php_cgi(client);
	else if (!client.server->compression.compare("on"))
		compressed_file_fd(client, client.request.file);
	else
	{
		client.read_fd = open(client.request.file.c_str(), O_RDONLY);
		client.request.file_size = info.st_size;
		client.request.res = generate_200_header(0, client.request.file, client.request.file_size);
	}
	if (!client.read_fd)
		load_error_page(client);
	ret = read(client.read_fd, buffer, MAX_SIZE);
	if (ret < MAX_SIZE)
	{
		if (client.read_fd)
			close(client.read_fd);
		delete_temp_file(client, client.temp_file);
		client.read_fd = 0;
	}
	client.request.res.append(buffer, ret);
	ft_send(client);
	if (client.read_fd == 0)
		return (false);
	return (true);
}

bool					ft_post(t_client &client)
{
	std::string multipart("multipart/");
	std::string boundary("; boundary=");
	std::map<std::string, std::string>::iterator it;
	std::map<std::string, std::string>::iterator it2;
	std::map<std::string, std::string>::iterator it3;
	char buffer[MAX_SIZE + 1];
	int ret;
	struct stat info;
	int fd;
	std::string temp(".temp_gzip_chunk");
	
	stat(client.request.file.c_str(), &info);
	client.request.pt_data.on = 1;
	client.request.pt_data.end = 0;
	client.request.pt_data.size = 0;
	it = client.request.headers.find("Transfer-Encoding");
    it2 = client.request.headers.find("Content-Length");
	it3 = client.request.headers.find("Content-Encoding");
    if (it != client.request.headers.end() && (!it->second.compare("chunked") || !it->second.compare("chunked, gzip") || !it->second.compare("gzip, chunked")))
    {
        client.request.pt_data.size = -1;
        client.request.pt_data.end = unchunk_data(client);
		if (client.request.request.size() > 65000)
		{
			client.request.res = "HTTP/1.1 413 Payload Too Large\\r\n\r\n";
        	client.read_fd = 0;
			client.request.pt_data.on = 0;
			return (false);
		}
		else if (!is_end(client))
		{
			client.request.res.clear();
			if (client.read_fd)
				close(client.read_fd);
			return (false);
		}
		else if (client.request.pt_data.end == -1)
		{
			ft_send(client);
        	client.request.pt_data.on = 0;
			client.disconnect = 1;
			return (false);
		}
    }
    else
        client.request.pt_data.size = std::stoi(it2->second);
	if ((it != client.request.headers.end() && (!it->second.compare("gzip") || !it->second.compare("chunked, gzip") || !it->second.compare("gzip, chunked")))
    || (it3 != client.request.headers.end() && (!it3->second.compare("gzip"))))
    {
		client.request.pt_data.compress_req += client.request.request;
		if (client.request.pt_data.compress_req.size() > 65000)
		{
			client.request.res = "HTTP/1.1 413 Payload Too Large\\r\n\r\n";
			if (client.read_fd)
				close(client.read_fd);
        	client.read_fd = 0;
			client.request.pt_data.on = 0;
			return (false);
		}
        if (!client.request.pt_data.end && ((int)client.request.pt_data.compress_req.size() < client.request.pt_data.size || (client.request.pt_data.size == -1)))
			return (false);
		fd = open(temp.c_str(), O_RDWR | O_CREAT | O_TRUNC, 0664);
		write(fd, client.request.pt_data.compress_req.c_str(), client.request.pt_data.compress_req.size());
		close(fd);
		client.request.request = ft_inflate_file(temp);
    }
    if (client.request.pt_data.end == -1)
    {
        ft_send(client);
        client.request.pt_data.on = 0;
    }
	client.request.bytes_read += client.request.request.size();
	if (info.st_mode & S_IFDIR || !client.request.file.compare(""))
		ft_listing(client);
	else if (client.request.loc.php.compare("off") &&  check_php_extension(client.request.file))
		ft_php_cgi(client);
	else if (!client.server->compression.compare("on"))
		compressed_file_fd(client, client.request.file);
	else
	{
		client.read_fd = open(client.request.file.c_str(), O_RDONLY);
		client.request.file_size = info.st_size;
		client.request.res = generate_200_header(0, client.request.file, client.request.file_size);
	}
	if (!((client.request.pt_data.size >= 0 && client.request.pt_data.size <= client.request.bytes_read) || ((int)client.request.pt_data.compress_req.size() >= client.request.pt_data.size) ||  client.request.pt_data.end))
	{
		client.request.res.clear();
		close(client.read_fd);
		return (false);
	}
	if (!client.read_fd && !client.request.pt_data.on)
		load_error_page(client);
	client.request.pt_data.on = 0;
	ret = read(client.read_fd, buffer, MAX_SIZE);
	if (ret < MAX_SIZE)
	{
		close(client.read_fd);
		delete_temp_file(client, client.temp_file);
		client.read_fd = 0;
	}
	client.request.res.append(buffer, ret);
	ft_send(client);
	client.request.request.clear();
	client.request.bytes_read = 0;
	if (client.read_fd == 0)
		return (false);
	return (true);
}
