#include "../webserver.hpp"

bool					ft_put(t_client &client)
{
    int fd;
	char buffer[BUFFER_SIZE + 1];
	std::string file_name;
    std::map<std::string, std::string>::iterator it;
    std::map<std::string, std::string>::iterator end;
    std::map<std::string, std::string>::iterator it2;
    std::map<std::string, std::string>::iterator it3;

	fd = open(client.request.file.c_str(), O_RDONLY);
    client.request.pt_data.end = 0;
    it = client.request.headers.find("Transfer-Encoding");
    it2 = client.request.headers.find("Content-Length");
    it3 = client.request.headers.find("Transfer-Encoding");
    if ((it != client.request.headers.end() && (!it->second.compare("chunked") || !it->second.compare("chunked, gzip") || !it->second.compare("gzip, chunked")))
    || (it3 != client.request.headers.end() && (!it3->second.compare("chunked") || !it3->second.compare("chunked, gzip") || !it3->second.compare("gzip, chunked"))))
    {
        client.request.pt_data.size = -1;
        client.request.pt_data.end = unchunk_data(client);
    }
    else
        client.request.pt_data.size = std::stoi(it2->second);
    if (client.request.pt_data.end == -1)
    {
        close(fd);
        ft_send(client);
        client.request.pt_data.on = 0;
    }
	if (fd > 0)
	{
        file_name = client.request.file;
		close(fd);
		client.request.res = "HTTP/1.1 204 No Content\r\nDate: " + get_date() + "\r\nServer: Webserver/1.0\r\nContent-Location : " +
			client.request.file + "\r\n\r\n";
        if (client.request.pt_data.on)
		    fd = open(client.request.file.c_str(), O_RDWR | O_APPEND);
        else
            fd = open(client.request.file.c_str(), O_TRUNC | O_RDWR);
        client.request.pt_data.on = 1;
		client.request.bytes_read += write(fd, client.request.request.c_str(), client.request.request.size());
		close(fd);
	}
	else
	{
		if (client.request.loc.save.compare(""))
		{
			file_name = get_file_name(client.request.file.c_str());
			getcwd(buffer, BUFFER_SIZE);
			chdir(client.server->root.c_str());
			chdir(client.request.loc.save.c_str());
            file_name = client.request.loc.save + "/" + file_name;
			client.request.res = "HTTP/1.1 204 No Content\r\nDate: " + get_date() + "\r\nServer: Webserver/1.0\r\nContent-Location: " +
				file_name + "\r\n\r\n";
		}
		else
		{
			chdir(client.request.loc.location.c_str());
			file_name = client.request.file;
			client.request.res = "HTTP/1.1 204 No Content\r\nDate: " + get_date() + "\r\nServer: Webserver/1.0\r\nContent-Location: " +
				file_name + "\r\n\r\n";
		}
        if (client.request.pt_data.on)
		    fd = open(client.request.file.c_str(), O_RDWR | O_APPEND);
        else
            fd = open(client.request.file.c_str(), O_CREAT | O_RDWR, 0666);
        client.request.pt_data.on = 1;
        client.request.bytes_read += write(fd, client.request.request.c_str(), client.request.request.size());
		chdir(buffer);
	}
    if ((client.request.pt_data.size >= 0 &&  client.request.pt_data.size <= client.request.bytes_read) ||  client.request.pt_data.end)
    {
	    ft_send(client);
        client.request.pt_data.on = 0;
        client.request.bytes_read = 0;
        it = client.request.headers.find("Content-Encoding");
        it2 = client.request.headers.find("Transfer-Encoding");
        end = client.request.headers.end();
        if ((it != end && (!it->second.compare("chunked, gzip") || !it->second.compare("gzip") || !it->second.compare("gzip, chunked")))
        || (it2 != end && (!it2->second.compare("chunked, gzip") || !it2->second.compare("gzip") || !it2->second.compare("gzip, chunked"))))
            ft_inflate_file_fd(file_name);
        return (false);
    }
    return (true);
}

bool					ft_connect(t_client &client)
{
    client.request.res = "HTTP/1.1 400 Bad Request\r\n\r\n";
	client.disconnect = 1;
    return (true);
}

bool					ft_trace(t_client &client)
{

    std::string rqst = client.request.header_string + client.request.request;

    client.request.res = "HTTP/1.1 200 OK\nContent-Type: message/http\nContent-Length: ";
    client.request.res += ft_int_to_string(rqst.size());
    client.request.res += "\n\n";
    client.request.res+= rqst;
    ft_send(client);
    return (false);
}

bool					ft_head(t_client &client)
{
    struct stat info;

    stat(client.request.file.c_str(), &info);
    if (info.st_mode & S_IFDIR || !client.request.file.compare(""))
        ft_listing(client);
    else if (!client.request.loc.php.compare("on") && check_php_extension(client.request.file))
        ft_php_cgi(client);
    else if (!client.server->compression.compare("on"))
    {
        compressed_file_fd(client, client.request.file);
        client.request.res = generate_200_header(1, client.request.file, client.request.file_size);
    }
    else
    {
        stat(client.request.file.c_str(), &info);
        client.read_fd = open(client.request.file.c_str(), O_RDONLY);
        client.request.file_size = info.st_size;
        client.request.res = generate_200_header(0, client.request.file, client.request.file_size);
    }
    if (!client.read_fd)
    {
        load_error_page(client);
        return (false);
    }
    close(client.read_fd);
    client.read_fd = 0;
    ft_send(client);
    return (false);
}
