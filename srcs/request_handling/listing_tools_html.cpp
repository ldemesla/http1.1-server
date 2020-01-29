#include "../webserver.hpp"

void	ft_add_header(std::string &final_body, std::string &title, std::string &balise_h1)
{
	final_body += "<!DOCTYPE html>\n";
	final_body += "<html lang=\"fr\">";
	final_body += "<head>\n";
	final_body += "\t<meta charset=\"UTF-8\">\n";
	final_body += "\t<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n";
	final_body += "\t<meta http-equiv=\"X-UA-Compatible\" content=\"ie=edge\">\n";
	final_body += "\t<title>" + title + "</title>\n";
	final_body += "</head>\n";
	final_body += "<body>\n";
	final_body += "\t<h1>" + balise_h1 + "</h1>\n<hr>\n";
}

void	ft_add_bottom(std::string &final_body, int port)
{
	final_body += "</body>\n";
	final_body += "<bottom>\n<hr>\n";
	final_body += "\t<p>Webserv/1.7 (macOS Mojav/10.14.5) Server at localhost Port ";
	final_body += std::to_string(port);
	final_body += "</p>";
}

void	ft_add_balise_a(std::string &final_body, const std::string &path, const std::string &temp_1)
{
	final_body += "\t";
	final_body += "<p>folder: <a href=\"";
	final_body += "/";
	final_body += path;
	final_body += "/";
	final_body += temp_1;
	final_body += "\">";
	final_body += "/";
	final_body += temp_1;
	final_body += "</a></p>";
	final_body += "\n";
}

void	ft_add_balise_p(std::string &final_body, const std::string &temp_1)
{
	final_body += "\t";
	final_body += "<p>file:     ";
	final_body += "/";
	final_body += temp_1;
	final_body += "</p>";
	final_body += "\n";
}
