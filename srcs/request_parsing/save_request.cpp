#include "../webserver.hpp"

bool check_method_exist(t_request &request)
{
    int i;
    int k;
    t_methods methods;
    std::string temp;
    std::string version("HTTP/1.1");
    
    i = 0;
    while (i < METHODS_NBR)
    {
        if (!request.request.compare(0, methods.methods[i].size(), methods.methods[i]))
        {
            request.method = request.request.substr(0, methods.methods[i].size() - 2);
            k = 0;
            while (request.request[k] != '\r')
                k++;
            temp = request.request.substr(request.method.size() + 1, i - (request.method.size() + 1));
            k = 0;
            while (temp[k] && temp[k] != ' ')
                k++;
            if (!temp[k] || temp[k] != ' ' || temp.find(version) == (unsigned long)-1)
                i = METHODS_NBR;
            break ;
        }
        i++;
    }
    if (i == METHODS_NBR)
    {
        request.res = "HTTP/1.1 400 Bad Request\r\n\r\n";
        return (false);
    }
    return (true);
}

bool check_method_allowed(t_client &client)
{
    std::vector<std::string>::iterator end;
    std::vector<std::string>::iterator it;
    if (!get_location(client))
    {
        load_error_page(client);
        return (false);
    }
    end = client.request.loc.methods.end();
    it = client.request.loc.methods.begin();
    while (it != end)
    {
        if (!client.request.method.compare(*it))
            break ;
        it++;
    }
    if (it == end)
    {
        client.request.res = "HTTP/1.1 405 Method Not Allowed\r\n\r\n";
        return (false);
    }
    return (true);
}

bool file_exit(t_client &client)
{
    struct stat	info;
    std::string error_page;
    std::string path;

    chdir(client.request.loc.root.c_str());
    stat(client.request.file.c_str(), &info);
    if (info.st_mode & S_IFREG)
        return (true);
    else if ((info.st_mode & S_IFDIR || !client.request.file.compare("")) && client.request.loc.listing.compare("on"))
    {
        std::vector<std::string>::iterator it = client.request.loc.index.begin();
        std::vector<std::string>::iterator end= client.request.loc.index.end();
        while (it != end)
        {
            path = client.request.file + *it;
            stat(path.c_str(), &info);
            if (info.st_mode & S_IFREG)
            {
                client.request.file = path;
                return (true);
            }
            it++;
        }
    }
    else if ((!client.request.file.compare("") && !client.request.loc.listing.compare("on")) || !client.request.method.compare("PUT")
    || (info.st_mode & S_IFDIR && !client.request.loc.listing.compare("on")))
    {
        if (!client.request.method.compare("PUT") && ((info.st_mode & S_IFDIR) || !client.request.file.size()))
        {
            client.request.res = "HTTP/1.1 400 Bad Request\r\n\r\n";
            client.read_fd = 0;
            return (false);
        }
        return (true);
    }
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
    return (false);
}

void remove_headers(t_request &request)
{
    int i;
    int size;

    i = 0;
    size = request.request.size();
    while (i < size)
    {
          if (i >= 3 && request.request[i - 3] == '\r' && request.request[i - 2] == '\n' && request.request[i - 1] == '\r' && request.request[i] == '\n')
			break ;
        i++;
    }
    if (i + 1 < size)
    {
        i++;
        request.header_string = request.request.substr(0, i);
        request.request = request.request.substr(i, size);
    }
    else
    {
        request.header_string = request.request;
        request.request.clear();
    }
}

bool get_info(t_client &client)
{
    std::map<std::string, std::string>::iterator it;
    std::map<std::string, std::string>::iterator it2;
    std::map<std::string, std::string>::iterator end;

    if (!check_method_exist(client.request) || !check_method_allowed(client)
    || !file_exit(client) ||  !parse_header(client.request, client.server))
        return (false);
    remove_headers(client.request);
    it = client.request.headers.find("Content-Length");
    it2 = client.request.headers.find("Transfer-Encoding");
    if (!client.request.request.empty() && it == end && it2 == end)
    {
        client.request.res = "HTTP/1.1 400 Bad Request\r\n\r\n";
        return (false);
    }
    return (true);
}


bool ft_save_request(char *buffer, std::vector<t_client>::iterator &it, int ret)
{
    int i;
    int header = 0;

    i = 0;
    if (!it->request.pt_data.on)
    {
        it->request.request.append(buffer, ret);
        while (it->request.request[i])
        {
            if (i >= 3 && it->request.request[i - 3] == '\r' && it->request.request[i - 2] == '\n' && it->request.request[i - 1] == '\r' && it->request.request[i] == '\n')
            {
                header = 1;
			    break ;
            }
            i++;
        }
        if (header == 1)
        {
            if (!get_info(*it))
                it->disconnect= 1;
            else
                it->disconnect = 0;
        }
        else
        {
            chdir(it->server->root.c_str());
            return (false);
        }
    }
    else
        it->request.request.append(buffer, ret);
    chdir(it->server->root.c_str());
    return (true);
}