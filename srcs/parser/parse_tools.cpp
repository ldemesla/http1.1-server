#include "../webserver.hpp"

std::string ft_cut_line_string(std::string &line_string)
{
    std::string new_line_string(line_string);
    for (std::string::iterator it(new_line_string.begin()); it != new_line_string.end(); it++)
        if (*it == ' ')
        {
            new_line_string.erase(it, new_line_string.end());
            break;
        }
    return (new_line_string);
}

std::vector<std::string> ft_get_methods(std::string line)
{
    std::vector<std::string> methods;
    int i(0);
    int j;

    while (line[i] && (line[i] == ' ' || line[i] == '\t'))
        i++;
    while (line[i] && line[i] != ' ' && line[i] != '\t')
        i++;
    while (line[i])
    {
        while (line[i] && (line[i] == ' ' || line[i] == '\t'))
            i++;
        j = i;
        while (line[i] <= 'Z' && line[i] >= 'A')
            i++;
        methods.push_back(line.substr(j, i - j));
        i++;
    }
    return (methods);
}

std::vector<std::string>	ft_get_index(std::string line)
{
    std::vector<std::string> index;
    int i(0);
    int j;
    
    while (line[i] && (line[i] == ' ' || line[i] == '\t'))
        i++;
    while (line[i] && line[i] != ' ' && line[i] != '\t')
        i++;
    while (line[i] && line[i] != ';')
    {
        while (line[i] && line[i] != ';' && (line[i] == ' ' || line[i] == '\t'))
            i++;
        j = i;
        while (line[i] && line[i] != ';' && line[i] != ' ' && line[i] != '\t')
            i++;
        index.push_back(line.substr(j, i - j));
    }
    line = line.substr(i, line.size() - 1);
    return (index);
}

std::string ft_get_val(std::string line)
{
    int i;
    int j;
    i = 0;
    while (line[i] && line[i] == '\t')
        i++;
    while (line[i] && line[i] != ' ' && line[i] != '\t')
        i++;
    while (line[i] && (line[i] == ' ' || line[i] == '\t'))
        i++;
    j = i;
    while (line[i] && line[i] != ' ' && line[i] != ';')
        i++;
    return (line.substr(j, i -j));
}
 
int ft_get_int(std::string line)
{
    int i;
    int j;
    i = 0;
    while (line[i] && line[i] == '\t')
        i++;
    while (line[i] && line[i] != ' ' && line[i] != '\t')
        i++;
    while (line[i] && (line[i] == ' ' || line[i] == '\t'))
        i++;
    if (line[i] < '0' || line[i] > '9')
        return (-1);
    j = i;
    while (line[i] >= '0' && line[i] <= '9' && line[i])
        i++;
    if (line[i] != ';')
        return (-1);
    return (std::stoi(line.substr(j, i -j)));
}