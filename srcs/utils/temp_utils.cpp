#include "../webserver.hpp"

int open_temp_file(t_client client, std::string temp_file, int mode, int acces)
{
    char buffer[BUFFER_SIZE];
    int fd;

    getcwd(buffer, BUFFER_SIZE);
    chdir(client.server->root.c_str());
    if (chdir(client.temp_dir.c_str()) == -1)
    {
        mkdir(client.temp_dir.c_str(), ACCESSPERMS);
        chdir(client.temp_dir.c_str());
    }
    if (acces)
        fd = open(temp_file.c_str(), mode, 0666);
    else
        fd = open(temp_file.c_str(), mode, 0666);
    chdir(buffer);
    return (fd);
}

void delete_temp_file(t_client &client, std::string temp_file)
{
    char buffer[BUFFER_SIZE];   

    getcwd(buffer, BUFFER_SIZE);
    chdir(client.server->root.c_str());
    if (chdir(client.temp_dir.c_str()) == -1)
    {
        chdir(buffer);
        return ;
    }
    unlink(temp_file.c_str());
    chdir(buffer);
}

int stat_temp(t_client &client, struct stat *info, std::string file)
{
    char buffer[BUFFER_SIZE];
    int ret;

    getcwd(buffer, BUFFER_SIZE);
    chdir(client.server->root.c_str());
    if (chdir(client.temp_dir.c_str()) == -1)
    {
        chdir(buffer);
        return -1;
    }
    ret = stat(file.c_str(), info);
    chdir(buffer);
    return (ret);
}