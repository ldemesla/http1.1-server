#include "../webserver.hpp"

bool						ft_is_hidden_file(char *d_name)
{
	return (d_name[0] == '.');
}

std::vector<std::string>	ft_list_file(std::string path)
{
	std::string					temp_name;
	std::vector<std::string>	list_file;
	DIR							*directory(opendir(path.c_str()));
	struct dirent				*directory_entry;
	
	if (directory != NULL)
	{
		while ((directory_entry = readdir(directory)) != NULL)
		{
			if (ft_is_hidden_file(directory_entry->d_name) == false)
			{
				if (directory_entry->d_type != DT_DIR)
					temp_name = "F";
				else
					temp_name = "D";
				temp_name += directory_entry->d_name;
				list_file.push_back(temp_name);
			}
		}
	}
	closedir(directory);
	return (list_file);
}

std::string	ft_generate_listing_page(t_client &client)
{
	std::string final_body;
	
	std::string	title("listing page");
	std::string	balise_h1("Index of /" + client.request.file);
	ft_add_header(final_body, title, balise_h1);
	std::vector<std::string>	list_file(ft_list_file(client.request.file));
	for (std::vector<std::string>::iterator it(list_file.begin()); it != list_file.end(); it++)
	{
		if (it->operator[](0) == 'F')
			ft_add_balise_p(final_body, it->substr(1, it->size() - 1));
		else
			ft_add_balise_a(final_body, client.request.file, it->substr(1, it->size() - 1));
	}
	ft_add_bottom(final_body, client.server->port);
	return (final_body);
}
