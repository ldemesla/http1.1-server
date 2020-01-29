#include "../webserver.hpp"

int		error(std::string err, bool perror_mod)
{
	std::cerr << "error: " << err << std::endl;
	if (perror_mod)
		perror(err.c_str());
	exit(EXIT_FAILURE);
}