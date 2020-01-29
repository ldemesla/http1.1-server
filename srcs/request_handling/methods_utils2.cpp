#include "../webserver.hpp"

void                    get_range(t_client &client)
{
    std::map<std::string, std::string>::iterator it;
    std::map<std::string, std::string>::iterator end;
    t_range range;
    std::string unit("bytes=");
    int i;

    end = client.request.headers.end();
    it = client.request.headers.find("If-Range");
    if (it != end && !modified_since(it->second, client.request.file) && !if_match(it->second, client.request.file))
        return ;
    else
    {
        it = client.request.headers.find("Range");
        if (it == end)
            return;
        else
        {
            if (it->second.compare(0, unit.size(), unit))
                return ;
            i = unit.size() + 1;
            while (it->second[i])
            {
                range.clear();
                while (it->second[i] <= '9' && it->second[i] >= '0')
                    range.start = range.start * 10 + (it->second[i++] - 48);
                i++;
                while (it->second[i] <= '9' && it->second[i] >= '0')
                    range.end = range.end * 10 + (it->second[i++] - 48);
                client.request.ranges.push_back(range);
                if (it->second[i] == ',')
                    i++;
                while (it->second[i] && it->second[i] == ' ')
                    i++;
            }
            
        }
           
    }
}

int range_len(t_client client)
{
    int size = 0;
    std::vector<t_range>::iterator it = client.request.ranges.begin();
    std::vector<t_range>::iterator end = client.request.ranges.end();
    while (it != end)
    {
        size += it->end - it->start;
        it++;
    }
    return (size);
}

std::string generate_206_header(int gzip, std::string file, unsigned long content_len)
{
    std::string header("HTTP/1.1 206 Partial Content\r\nContent-Language: en-US\r\nAccept-Ranges: None\r\nDate: " + get_date() +"\r\nServer: Webserver/1.0\r\nContent-Length: " + ft_long_to_string(content_len) + "\r\nContent-Type: " +  get_content_type(file) + "\r\n");
	if (gzip)
        header += "Content-Encoding: gzip\r\n";
	header += "\r\n";
    return (header);
}

bool format_res_range(t_client &client)
{
    t_range range;
    std::string res;
    int size;
    int i;

    range = client.request.ranges.front();
    size = client.request.res.size();
    if (range.end == 0)
        range.end = client.request.file_size;
    i = client.request.bytes_read;
    while (i < range.start && i < size)
        i++;
    if (i == size)
        return (false);
    if (range.start == client.request.bytes_read)
    {
        res =  generate_206_header(0, client.request.file, range_len(client)) + "--separator\r\nContent-Type: " + get_content_type(client.request.file) + "\r\nContent-Range: bytes "
        + ft_int_to_string(range.start) + "-" + ft_int_to_string(range.end) + "/" + ft_int_to_string(client.request.file_size) + "\r\n\r\n";
    }
    while (i < range.end && i < size && client.request.res[i])
        res += client.request.res[i++];
    if (i == range.end)
        client.request.ranges.erase(client.request.ranges.begin());
    client.request.res = res;
    if (!client.request.ranges.size())
    {
        client.request.res += "\r\n--separator--";
        close(client.read_fd);
        client.read_fd = 0;
    }
    return (true);
}