#include "../webserver.hpp"

int get_size(std::string string, int pos)
{
    int i;
    std::string hex_size;

    i = pos;
    while (string[i] != '\r')
        i++;
    if (i - pos == 1)
        hex_size += string[pos];
    else
        hex_size = string.substr(pos, i);
    return (ft_hex_to_dec(hex_size));
}

void remove_crlf(std::string &s, int last_chunk, int &chunk_size)
{
    if (last_chunk)
    {
        s = s.substr(0, s.size() - 2);
        chunk_size = chunk_size - 2;
    }
    return;
}

bool is_chunk_size(std::string chunk, int i, t_client &client)
{
    std::string hex = "abcdef";
    int k;
    if (client.request.chunked.chunk_size != client.request.chunked.current_len)
        return (false);
    if (chunk[i] == '\r')
        return (false);
    while (chunk[i])
    {
        if (chunk[i] < '0' || chunk[i] > '9')
        {
            k = 0;
            while (k < (int)hex.size())
            {
                if (chunk[i] == hex[k++])
                {
                    break;
                }
            }
            if (k == (int)hex.size())
                break;
        }
        i++;
    }
    if (chunk[i] == '\r')
    {
        return (true);
    }
    return (false);
}

int unchunk_data(t_client &client)
{
    std::string ending("\r\n0\r\n");
    int ending_pos;
    int end = 0;
    int i = 0;
    int k = 0;
    int last_chunk;
    int chunk_size = 0;
    std::string new_request;

    if ((ending_pos = client.request.request.find(ending.c_str())) != -1)
        end = 1;
    else
        ending_pos = client.request.request.size() - 1;
    while (i < ending_pos)
    {
        last_chunk = 0;
        if (is_chunk_size(client.request.request, i, client))
        {
            if (k != chunk_size)
            {
                client.request.res = "HTTP/1.1 400 Bad Request\r\n\r\n";
                return (-1);
            }
            chunk_size = get_size(client.request.request, i) + 2;
            k = 0;
            last_chunk = 1;
            client.request.chunked.chunk_size += chunk_size;
            while (client.request.request[i] && client.request.request[i] != '\n')
                i++;
            if (client.request.request[i] != '\n')
            {
                client.request.res = "HTTP/1.1 400 Bad Request\r\n\r\n";
                return (-1);
            }
            i++;
        }
        remove_crlf(new_request, last_chunk, chunk_size);
        client.request.chunked.current_len += 1;
        new_request += client.request.request[i];
        k++;
        i++;
    }
    if (k != chunk_size)
    {
        client.request.res = "HTTP/1.1 400 Bad Request\r\n\r\n";
        return (-1);
    }
    client.request.request = new_request;
    if (end)
    {
        client.request.chunked.chunk_size = 0;
        client.request.chunked.current_len = 0;
    }
    return (end);
}