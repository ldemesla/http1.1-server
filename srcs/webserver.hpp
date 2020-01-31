#ifndef WEBSERVER_HPP
# define WEBSERVER_HPP

# define BUFFER_SIZE 					4096
# define METHODS_NBR 					8
# define MOD_GZIP_ZLIB_WINDOWSIZE		15
# define MOD_GZIP_ZLIB_CFACTOR   		9
# define GETCWD_BUFFER_SIZE				1000
# define COMPRESSED_BUFFER_SIZE			50000
# define DECOMPRESSED_BUFFER_SIZE		8192
# define SERVER_BUFFER_SIZE				4096
# define PUT_BUFFER_SIZE				8192
# define MAX_PORT						60000
# define MAX_SIZE						4096
# define HASH_BUFFER_SIZE   			64
# define STRONG_VALIDATION  			1
# define WEAK_VALIDATION				2
# define UPPER_CASE						1
# define LOWER_CASE						2
# define DEFAULT_KEY_PEM_PATH			"../certs/key.pem"
# define DEFAULT_CERT_PEM_PATH			"../certs/cert.pem"
# define DEFAULT_CONF_PATH				"data/default/default.conf"
# define MAX_HEADER_PHP_CGI				40

# define TIME_OUT_SELECT_SEC			0
# define TIME_OUT_SELECT_USEC			500000 /* 1000000 u_sec == 1 sec */
# define TIME_OUT_CLIENT_SEC			5
# define FORCE_TIME_OUT_CLIENT			30

# define INITAL_USED_FD					2
# define MAX_FD_AVAILABLE				256
# define NBR_FD_ALLOCATED_BY_CLIENT		4
# define FD_MARGIN						20
# define FD_LIMIT_ACCEPTED				(MAX_FD_AVAILABLE / NBR_FD_ALLOCATED_BY_CLIENT) - FD_MARGIN

# define LISTEN_MAX						1000

# define NEW_REQUEST					0
# define OLD_REQUEST					1

# include <arpa/inet.h>
# include <errno.h>
# include <netinet/in.h>
# include <signal.h>
# include <stdio.h>
# include <sys/types.h>
# include <sys/stat.h>
# include <fcntl.h>

# include <iostream>
# include <fstream>
# include <stdlib.h>
# include <strings.h>
# include <sys/socket.h>
# include <dirent.h>
# include <unistd.h>
# include <string>
# include <vector>
# include <errno.h>
# include <assert.h>
# include <openssl/ssl.h>
# include <openssl/err.h>
# include <sys/uio.h>
# include <map>
# include <iterator>
# include <sys/time.h>
# include "get_next_line/get_next_line.hpp"
# include <zlib.h>
# include <iomanip>
# include <sys/uio.h>
# include <openssl/sha.h>



typedef struct s_location
{
	std::string 				location;
	std::string 				root;
	std::vector<std::string>	methods;
	std::string					listing;
	std::string					save;
	std::string					php;
	std::vector<std::string>	index;
}				t_location;

typedef struct	s_server
{
	int							fd;
	SSL							*ssl_fd;
	int							port;
	std::string					compression;
	std::string					TLS;
	std::string					key_path;
	std::string					cert_path;
	int							body_size;
	std::string					error_page;
	std::string					host;
	std::vector<t_location>		location;
	std::string					root;
	struct sockaddr_in			addr;
	SSL_CTX						*ctx;
}				t_server;

typedef struct s_range
{
	void clear()
	{
		start = 0;
		end = 0;
	}
	int start;
	int end;
}				t_range;

typedef struct s_pt
{	
	s_pt()
	{
		on = 0;
	}
	std::string cache;
	std::string boundary;
	int			end;
	int			size;
	int			on;
	std::string compress_req;
} t_pt;

typedef struct s_chunked
{
	s_chunked()
	{
		chunk_size = -1;
		i = 0;
		end = 0;
	}
	int chunk_size;
	int i;
	int end;
	std::string cache;
}t_chunked;

typedef struct s_request
{
	s_request()
	{
		bytes_read = 0;
	}
	std::vector<t_range>				ranges;
	std::string							request;
	std::string 						header_string;
	t_location							loc;
	std::string							file;
	std::string							method;
	std::map<std::string, std::string>	headers;
	int									bytes_read;
	std::string							res;
	unsigned long						file_size;
	t_pt								pt_data;
	t_chunked							chunked;
}				t_request;

typedef struct s_finish
{
	std::string boundaries;
	int chunked;
	int len;
	int body;
	int bad_request;
	int not_implemented;
	int expect_continue;
}			t_finish;

typedef struct  s_etag
{
	std::string hash;
	int		validation_mod;
}               t_etag;

std::string 			ft_int_to_string(int nbr);

typedef struct	s_client
{
	s_client()
	{
		read_fd = 0;
		temp_dir = ".temp";
	}
	int							fd;
	int							read_fd;
	int							connection_stat;
	SSL							*ssl_fd;
	t_server					*server;
	t_request					request;
	int							disconnect;
	std::string					temp_dir;
	std::string					temp_file;
	struct timeval				last_action;
}				t_client;

typedef struct s_cleaner
{
	std::vector<t_server>		*servers;
	std::vector<t_client>		*clients;
	fd_set						*ini_set_read;
	fd_set						*ini_set_write;
}				t_cleaner;

extern t_cleaner				g_cleaner;
extern std::string				g_initial_path;

int 						unchunk_data(t_client &client);
void						ft_sigint_signal_catcher(int n);
void						ft_sigpipe_signal_catcher(int n);

int		open_temp_file(t_client client, std::string temp_file, int mode, int acces);
void 	delete_temp_file(t_client &client, std::string temp_file);
int 	stat_temp(t_client &client, struct stat *info, std::string file);

void						compressed_file_fd(t_client &client, std::string file);
std::string					ft_inflate_file(const std::string fileName);
void						ft_inflate_file_fd(const std::string fileName);

std::string 				generate_200_header(int gzip, std::string file, unsigned long content_len);
std::string 				generate_206_header(int gzip, std::string file, unsigned long content_len);
bool    					ft_request_handling(t_client &client);
bool						ft_options(t_client &client);
bool						ft_delete(t_client &client);
bool						ft_get(t_client &client);
bool						ft_post(t_client &client);
bool						ft_put(t_client &client);
bool						ft_connect(t_client &client);
bool						ft_trace(t_client &client);
bool						ft_head(t_client &client);
void						ft_listing(t_client &client);
std::string 				get_file_name(std::string file);
void						get_range(t_client &client);
bool 						format_res_range(t_client &client);
std::string 				generate_error_page(int code);
std::string 				ft_get_features(const std::string &file_name, const int mod);
std::string 				generate_304_header(int gzip, std::string file);
bool 						is_end(t_client client);

bool 						ft_save_request(char *buffer, std::vector<t_client>::iterator &it, int ret);
bool						get_location(t_client &client);
bool						if_match(const std::string &line, const std::string &file_name);
std::vector<t_etag>  		ft_parse_line_etag(const std::string &line);
std::string 				ft_get_features(const std::string &file_name, const int mod);
std::string 				ft_sha256(const std::string source);
bool 						modified_since(const std::string &header, const std::string &file);
std::string 				set_header(t_client &client, std::string file);
void 						load_error_page(t_client &client);

typedef void(*server_set_function)(t_server &server, std::string &line_string);
typedef void(*location_set_function)(t_location &location, std::string &line_string);

int							error(std::string err, bool perror_mod);
void						ft_server(std::vector<t_server> &servers);
void						ft_init_fd_set(std::vector<t_server> &server, fd_set &ini_set_read, fd_set &ini_set_write);
void						ft_init_server(std::vector<t_server> *server, char **av);
bool 						parse_header(t_request &request, t_server *server);
void 						ft_parse_conf(std::vector<t_server> *server, char **av);
bool						ft_path_with_slash(char *path);

int							ft_strlen(char *s);
void						*ft_free(void *ptr);

void						ft_check_server_parsing(t_server &server);
void						ft_check_location_parsing(t_location &location);

t_location 					ft_add_location(int file, std::string line_string);
t_server 					ft_add_server(int file, std::string line_string);

std::string					ft_cut_line_string(std::string &line_string);
std::vector<std::string>	ft_get_methods(std::string line);
std::vector<std::string>	ft_get_index(std::string line);
std::string					ft_get_val(std::string line);
int							ft_get_int(std::string line);
std::string					get_content_type(std::string filename);
std::string 				set_header(t_client &client, std::string file);
int 						check_php_extension(std::string file);

void    					ft_set_server_port(t_server &server, std::string &line_string);
void    					ft_set_server_error_page(t_server &server, std::string &line_string);
void    					ft_set_server_host(t_server &server, std::string &line_string);
void    					ft_set_server_compression(t_server &server, std::string &line_string);
void    					ft_set_server_TLS(t_server &server, std::string &line_string);
void    					ft_set_server_body_size(t_server &server, std::string &line_string);
void    					ft_set_server_cert(t_server &server, std::string &line_string);
void						ft_set_server_key(t_server &server, std::string &line_string);

void    					ft_set_location_root(t_location &server, std::string &line_string);
void    					ft_set_location_methods(t_location &server, std::string &line_string);
void    					ft_set_location_listing(t_location &server, std::string &line_string);
void    					ft_set_location_save(t_location &server, std::string &line_string);
void    					ft_set_location_php(t_location &server, std::string &line_string);
void    					ft_set_location_index(t_location &server, std::string &line_string);

std::string					ft_get_get_var(const char *s);
std::string					ft_get_post_var(const char *s);
void 						ft_php_cgi(t_client &client);

std::string					ft_int_to_hex(int nbr, int mode);
std::string 				ft_int_to_string(int nbr);
std::string					get_file_date(const std::string &file);
std::string					ft_long_to_string(unsigned long nbr);
int							ft_hex_to_dec(const std::string &hex_str);

std::string 				get_date();
bool 						compare_date(std::string date1, std::string date2);

int							ft_send(t_client &client);
std::string					ft_cut_path(char *path);

void 						init_openssl();
void 						cleanup_openssl();
SSL_CTX						*create_context();
void						configure_context(SSL_CTX *ctx, t_server &server);
int							krx_ssl_verify_peer(int ok, X509_STORE_CTX* ctx);

void						ft_add_header(std::string &final_body, std::string &title, std::string &balise_h1);
void						ft_add_bottom(std::string &final_body, int port);
void						ft_add_balise_a(std::string &final_body, const std::string &path, const std::string &temp_1);
void						ft_add_balise_p(std::string &final_body, const std::string &temp_1);

bool						ft_is_hidden_file(char *d_name);
std::vector<std::string>	ft_list_file(std::string path);
std::string					ft_generate_listing_page(t_client &client);

void						ft_reset_last_action_client(t_client &client);
unsigned long 				ft_get_last_action_client(t_client &client);
int    						ft_timeout_clients(std::vector<t_client>::iterator it, std::vector<t_client> &clients, fd_set &ini_set_read, fd_set &ini_set_write);
std::string					ft_get_initial_path(void);

void						ft_disconnect_client(std::vector<t_client>::iterator &it, std::vector<t_client> &clients, fd_set &ini_set_read, fd_set &ini_set_write);
struct timeval				ft_init_timeout_select(time_t sec, time_t u_sec);

typedef struct	s_methods
{
	s_methods()
	{
		int i = 0;

		std::string tab[METHODS_NBR] = {"POST /", "GET /", "HEAD /", "PUT /", "DELETE /", "CONNECT /", "OPTIONS /", "TRACE /"};
		bool		(*f[METHODS_NBR])(t_client &) = {&ft_post, &ft_get, &ft_head, &ft_put, &ft_delete, 
			&ft_connect, &ft_options, &ft_trace};
		while (i < METHODS_NBR)
		{
			methods.push_back(tab[i]);
			func.push_back(f[i]);
			i++;
		}
	}
	std::vector<std::string>			methods;
	std::vector<bool (*)(t_client &)>	func;
}				t_methods;

#endif
