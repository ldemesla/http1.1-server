#include "../webserver.hpp"

std::string ft_header_upper(std::string header)
{
	std::string new_header;

	for (unsigned long i(0); i < header.size(); i++)
	{
		if (header[i] >= 'a' && header[i] <= 'z')
			new_header += header[i] - 32;
		else if (header[i] != '-')
			new_header += header[i];
		else
			new_header += '_';
	}
	return (new_header);
}

std::string ft_get_get_var(const char *s)
{
	std::string get_var;

	if (s != NULL)
	{
		int i(0);

		while (s[i] && s[i] != ' ')
			i++;
		while (s[i] && s[i] == ' ')
			i++;
		while (s[++i] && s[i] != ' ' && s[i] != '\r' && s[i] != '\n')
			if (s[i] == '?')
				break;
		if (s[i] == '?')
		{
			i++;
			while (s[i] && s[i] != '\r' && s[i] != '\n' && s[i] != ' ')
				get_var += s[i++];
		}
	}
	return (get_var);
}

std::string ft_get_post_var(const char *s)
{
	std::string post_var;
	if (s != NULL)
	{
		int i(0);
		while (s[i])
			post_var.push_back(s[i++]);
	}
	return (post_var);
}

void ft_add_request_header_to_vector(t_client &client, std::vector<char *> &vector_envp)
{
	int i(0);
	static std::string string_KEY[MAX_HEADER_PHP_CGI];

	for (std::map<std::string, std::string>::iterator it(client.request.headers.begin()); i < MAX_HEADER_PHP_CGI && it != client.request.headers.end(); it++)
	{
		string_KEY[i] = ft_header_upper(it->first);
		string_KEY[i] += '=';
		string_KEY[i] += it->second;
		vector_envp.push_back((char *)string_KEY[i].c_str());
		i++;
	}
}

std::vector<char *> ft_set_vector_envp(t_client &client, std::string &input_file, std::string &input_get_args, std::string &input_post_args)
{
	static std::string string_SCRIPT_FILENAME;
	static std::string string_QUERY_STRING;
	static std::string string_CONTENT_TYPE;
	static std::string string_REDIRECT_STATUS;
	static std::string string_REQUEST_METHOD;
	static std::string string_CONTENT_LENGTH;

	std::vector<char *> vector_envp;
	if (input_get_args.empty() == false || input_post_args.empty() == false)
	{
		ft_add_request_header_to_vector(client, vector_envp);
		string_SCRIPT_FILENAME = "SCRIPT_FILENAME=" + input_file;
		vector_envp.push_back((char *)string_SCRIPT_FILENAME.c_str());
		if (input_get_args.empty() == false)
		{
			string_QUERY_STRING = "QUERY_STRING=" + input_get_args;
			vector_envp.push_back((char *)string_QUERY_STRING.c_str());
		}
		if (input_post_args.empty() == false)
		{
			string_REDIRECT_STATUS = "REDIRECT_STATUS=";
			string_REDIRECT_STATUS += "true";
			vector_envp.push_back((char *)string_REDIRECT_STATUS.c_str());
			string_REQUEST_METHOD = "REQUEST_METHOD=";
			string_REQUEST_METHOD += "POST";
			vector_envp.push_back((char *)string_REQUEST_METHOD.c_str());
			if (client.request.header_string.find("Content-Length") == (unsigned long)-1 && client.request.header_string.find("chunked") != (unsigned long)-1)
			{
				string_CONTENT_LENGTH = "CONTENT_LENGTH=";
				string_CONTENT_LENGTH += ft_int_to_string(input_post_args.size());
				vector_envp.push_back((char *)string_CONTENT_LENGTH.c_str());
			}
		}
	}
	vector_envp.push_back(NULL);
	return (vector_envp);
}

void ft_php_cgi(t_client &client)
{
	int temp_fd;
	int pipe_fd[2];
	pid_t pid;
	std::string file;
	std::string path(client.request.loc.php);
	struct stat php_cgi_stat;
	struct stat info;
	std::string get_args;
	std::string post_args;
	std::string nothing;
	std::string flag("-q");
	std::vector<char *> vector_envp;

	post_args += ft_get_post_var(client.request.request.c_str());
	if (client.request.method.compare("GET") && (client.request.pt_data.size > client.request.bytes_read || client.request.pt_data.size == -1) && !client.request.pt_data.end)
		return;
	get_args = ft_get_get_var(client.request.header_string.c_str());
	vector_envp = ft_set_vector_envp(client, client.request.file, get_args, !client.request.method.compare("GET") ? nothing : post_args);
	if (!client.request.method.compare("POST"))
		file = client.temp_file + "_post";
	else if (!client.server->compression.compare("on"))
		file = client.temp_file + "_gzip";
	else
		file = client.temp_file;
	if ((temp_fd = open_temp_file(client, file, O_RDWR | O_CREAT | O_TRUNC, 1)) == -1)
		error("open() #1", true);
	char *envp[vector_envp.size()];
	for (int i(0); i < (int)vector_envp.size(); i++)
		envp[i] = vector_envp[i];
	pipe(pipe_fd);
	int ret_a_supperimer;
	if ((ret_a_supperimer = write(pipe_fd[1], post_args.c_str(), post_args.size())) < 0)
		error("write()", true);
	close(pipe_fd[1]);
	if ((pid = fork()) == 0)
	{
		dup2(temp_fd, 1);
		if (stat(client.request.loc.php.c_str(), &php_cgi_stat) != 0 ||
			!(php_cgi_stat.st_mode & S_IFREG))
		{
			std::cout << "\r\n\r\n<p>error PHP CGI not found</p>" << std::endl;
			exit(EXIT_FAILURE);
		}
		dup2(pipe_fd[0], 0);
		char *args[4] = {(char *)client.request.loc.php.c_str(), (char *)flag.c_str(), (char *)client.request.file.c_str(), NULL};
		execve(args[0], args, envp);
		exit(EXIT_FAILURE);
	}
	waitpid(pid, NULL, 0);
	close(pipe_fd[0]);
	close(temp_fd);
	if (!client.request.method.compare("POST"))
		file = set_header(client, file);
	if (!client.server->compression.compare("on"))
		compressed_file_fd(client, file);
	else
	{
		stat_temp(client, &info, file);
		client.request.file_size = info.st_size;
		client.read_fd = open_temp_file(client, file, O_RDONLY, 0);
		client.request.res = generate_200_header(0, client.request.file, client.request.file_size);
	}
	post_args.clear();
}