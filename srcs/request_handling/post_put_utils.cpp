#include "../webserver.hpp"

bool is_not_hex(char c)
{
    if ((c < '0' || c > '9') && c != 'a' && c != 'b' && c != 'c' && c != 'd' && c != 'e' && c != 'f')
        return (true);
    return (false);
}

bool get_chunk_size(t_client &client)
{
    int i;

    i = 0;
    while (client.request.chunked.cache[i])
    {
        if (is_not_hex(client.request.chunked.cache[i]))
            break ;
        i++;
    }
    if (!client.request.chunked.cache[i])
        return (true);
    else if (client.request.chunked.cache[i] != '\r')
        return (false);
    client.request.chunked.chunk_size = ft_hex_to_dec(client.request.chunked.cache.substr(0, i));
    i += 2;
    client.request.chunked.cache = client.request.chunked.cache.substr(i, client.request.chunked.cache.size());
    return (true);
}


bool is_chunk_complete(t_client &client)
{
    int i;
    int size;

    i = 0;
    size = client.request.chunked.cache.size();
    while (i < size && i < client.request.chunked.chunk_size + 2)
        i++;
    if (i == client.request.chunked.chunk_size + 2)
    {
        client.request.request += client.request.chunked.cache.substr(0, i - 2);
        client.request.chunked.cache = client.request.chunked.cache.substr(i, client.request.chunked.cache.size() - i);
        client.request.chunked.chunk_size = -1;
        client.request.chunked.i = client.request.request.size();
        return (true);
    }
    return (false);
}

bool chunk_processing(t_client &client)
{
    if (client.request.chunked.chunk_size == -1 && !get_chunk_size(client))
        return (false);
    if (client.request.chunked.chunk_size == 0)
    {
        client.request.chunked.end = 1;
        return (true);
    }
    if (is_chunk_complete(client))
         return (chunk_processing(client));
    return (true);
}

int unchunk_data(t_client &client)
{
    std::cout << client.request.request << std::endl;
    if (client.request.request.size() == 0)
        return (0);
    client.request.chunked.cache += client.request.request.substr(client.request.chunked.i, client.request.request.size() - client.request.chunked.i);
    client.request.request = client.request.request.substr(0, client.request.chunked.i);
    if (!chunk_processing(client))
    {
        client.request.res = "HTTP/1.1 400 Bad Request\r\nr\n";
        client.read_fd = 0;
		client.request.pt_data.on = 0;
        client.request.chunked.end = 1;
        return (-1);
    }
    if (client.request.chunked.end)
        return (1);
    return (0);
}