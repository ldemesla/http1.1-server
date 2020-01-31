#include "../webserver.hpp"

bool	body_size(std::string request, t_server *server)
{
	int i;
	int size;

	i = 0;
	size = request.size();
	while (i < size)
	{
		if (i >= 3 && request[i - 3] == '\r' && request[i - 2] == '\n' &&
				request[i - 1] == '\r' && request[i] == '\n')
			break ;
		i++;
	}
	if (size - i  - 1> server->body_size + 1)
		return (false);
	return (true);
}

bool check_error_header(t_request &request, t_server *server)
{
    std::map<std::string, std::string>::iterator it;
    std::map<std::string, std::string>::iterator end;
    std::string multipart("multipart/");

    end = request.headers.end();
    it = request.headers.find("Host");
    if (server->host.compare("") && (it == end || it->second.compare(server->host) != 0))
    {
		request.res = "HTTP/1.1 400 Bad Request\r\n\r\n";
		return (false);
    }
    it = request.headers.find("Transfer-Encoding");
    if (it != end && (it->second.compare("gzip") != 0 && it->second.compare("gzip, chunked") != 0 &&
    it->second.compare("chunked, gzip") != 0 && it->second.compare("chunked") != 0))
    {
        request.res = "HTTP/1.1 501 Not Implemented\r\n\r\n";
        return (false);
    };
    it = request.headers.find("Expires");
    if (it != end)
    {
        std::string date(get_date());
        if (compare_date(date, it->second))
        {
            request.res = "Expired";
            return (false);
        }
    }
    it = request.headers.find("If-Match");
    if (it != end)
    {
        if (!if_match(it->second, request.file))
        {
            it = request.headers.find("Range");
            if (it != end)
                request.res = "HTTP/1.1 416 Range Not Satisfiable\r\n\r\n";
            else
                request.res = "HTTP/1.1 412 Precondition Failed\r\n\r\n";
            return (false);
        }

    }
    it = request.headers.find("If-None-Match");
    if (it != end && if_match(it->second, request.file))
    {
        if (!request.method.compare("GET") || !request.method.compare("HEAD"))
            request.res = generate_304_header((!server->compression.compare("on") ? 1 : 0), request.file);
        else
            request.res = "HTTP/1.1 412 Precondition Failed\r\n\r\n";
        return (false);
    }
    it = request.headers.find("Content-Type");
    if (it != end && (!multipart.compare(0, multipart.size(), it->second)))
    {
        request.res = "HTTP/1.1 400 Bad Request\r\n\r\n";
        return (false);
    }
    it = request.headers.find("If-Modified-Since");
    if (it != end && !(!request.method.compare("GET") || !request.method.compare("HEAD")))
    {
        request.res = "HTTP/1.1 400 Bad Request\r\n\r\n";
        return (false);
    }
    if (it != end && !modified_since(it->second, get_file_date(request.file)) && request.method.compare("TRACE"))
    {
        request.res = generate_304_header((!server->compression.compare("on") ? 1 : 0), request.file);
        return (false);
    }
    it = request.headers.find("If-Unmodified-Since");
    if (it != end && modified_since(it->second, get_file_date(request.file)))
    {
        request.res = "HTTP/1.1 412 Precondition Failed\r\nLast-Modified: " + get_file_date(request.file) + "\r\n\r\n";
        return (false);
    }
    it = request.headers.find("Content-Length");
    if (it != end)
    {
        for (unsigned long i = 0; i < it->second.size(); i++)
        {
            if (it->second[i] < '0' || it->second[i] > '9')
            {
                request.res = "HTTP/1.1 400 Bad Request\r\n\r\n";
                return (false);
            }
            
        }
        if ((std::stoi(it->second) > 65000 && !request.method.compare("POST")) || 
        body_size(request.request, server) == false)
        {
            request.res = "HTTP/1.1 413 Payload Too Large\r\n\r\n";
            return (false);
        }
    }
    else if (!request.method.compare("PUT") || !request.method.compare("POST"))
    {
        std::string encoding[3] = {"chunked", "chunked, gzip", "gzip, chunked"};
        it = request.headers.find("Transfer-Encoding");
        if (it !=end && (!encoding[0].compare(0, encoding[0].size(), it->second) ||  !encoding[1].compare(0, encoding[1].size(), it->second)
        || !encoding[2].compare(0, encoding[2].size(), it->second)))
            return (true);
        request.res = "HTTP/1.1 400 Bad Request\r\n\r\n";
        return (false);
    }
    return (true);
}

bool parse_header(t_request &request, t_server *server)
{
    int i;
    int j;
    int k;
    int l;
    int size;
    std::string header[17] = {"Content-Encoding:", "Range:", "Content-Length:", "Transfer-Encoding:","Host:","If-Match:", "If-None-Match:",
    "Content-Type:", "If-Modified-Since:", "If-Unmodified-Since:", "If-Range:", "Expires:", "Accept-Charset:", "Accept-Encoding:"
    , "Accept-Language:", "Date:", "Referer:"};

    i = 0;
    size = request.request.size();
    while (i < size)
    {
        if (i >= 3 && request.request[i - 3] == '\r' && request.request[i - 2] == '\n' &&
				request.request[i - 1] == '\r' && request.request[i] == '\n')
			break ;
        else if (i + 2 < size && request.request[i] == '\r' && request.request[i + 1] == '\n' && request.request[i + 2] != '\r')
        {
            j = 0;
            while (j < 17)
            {
                k = i + 2 + header[j].size();
                if (k < size && !request.request.compare(i + 2, header[j].size(), header[j]))
                {
                    l = k;
                    while (l < size && request.request[l] != '\r')
                        l++;
                    request.headers[header[j].substr(0, header[j].size() - 1)] = request.request.substr(k + 1, l - k - 1);
                }
                j++;
            }
        }
        i++;
    }
    if (!check_error_header(request, server))
        return (false);
    return (true);
}