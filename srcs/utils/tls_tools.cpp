#include "../webserver.hpp"

int krx_ssl_verify_peer(int ok, X509_STORE_CTX* ctx)
{
    X509_STORE_CTX *ctx_copy;

    ctx_copy = ctx;
    ok = 0;
    return (1);
}

void init_openssl()
{ 
    SSL_load_error_strings();	
    OpenSSL_add_ssl_algorithms();
}

void cleanup_openssl()
{
    EVP_cleanup();
}

SSL_CTX *create_context()
{
    const SSL_METHOD *method;
    SSL_CTX *ctx;

    // method = SSLv23_server_method();
    method = TLS_server_method();

    ctx = SSL_CTX_new(method);
    if (!ctx) {
	perror("Unable to create SSL context");
	ERR_print_errors_fp(stderr);
	exit(EXIT_FAILURE);
    }

    return ctx;
}

void configure_context(SSL_CTX *ctx, t_server &server)
{
	bool	default_cert_path(true);
	bool	default_key_path(true);

	SSL_CTX_set_ecdh_auto(ctx, 1);
	
	/* cert */
	
	// if (chdir(server.root.c_str()) < 0)
		// error("chdir", true);
	
	if (server.cert_path.empty())
	{
		default_cert_path = false;
		server.cert_path = DEFAULT_CERT_PEM_PATH;
	}
    if (SSL_CTX_use_certificate_file(ctx, server.cert_path.c_str(), SSL_FILETYPE_PEM) <= 0)
	{
        ERR_print_errors_fp(stderr);
		if (default_cert_path)
			std::cerr << "default path is used for cert file (" << server.cert_path << ")" << std::endl;
		else
			std::cerr << "non default path is used for cert file (" << server.cert_path << ")" << std::endl;
		exit(EXIT_FAILURE);
    }

	/* key */
	if (server.key_path.empty())
	{
		default_key_path = false;
		server.key_path = DEFAULT_KEY_PEM_PATH;
	}
    if (SSL_CTX_use_PrivateKey_file(ctx, server.key_path.c_str(), SSL_FILETYPE_PEM) <= 0 )
	{
        ERR_print_errors_fp(stderr);
		if (default_key_path)
			std::cerr << "default path is used for key file (" << server.key_path << ")" << std::endl;
		else
			std::cerr << "non default path is used for key file (" << server.key_path << ")" << std::endl;
		exit(EXIT_FAILURE);
    }
}