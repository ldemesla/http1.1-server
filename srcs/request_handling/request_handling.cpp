#include "../webserver.hpp"

bool    ft_request_handling(t_client &client)
{
    int i;
    t_methods methods;
    i = 0;
    if (client.request.res.size() > 0)
        ft_send(client);
    else
    {
        while (i <  METHODS_NBR)
        {
            if (!methods.methods[i].compare(0, client.request.method.size(), client.request.method))
            {
                return (methods.func[i](client));
            }
            i++;
        }
    }
    return (false);
}