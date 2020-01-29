NAME = webserver

COMP_TLS = comp_tls

INCLUDE = -Iincludes/openssl/include

OPENSSL =  includes/openssl/libssl.a includes/openssl/libcrypto.a -lz

SRCS =	srcs/parser/parse.cpp\
			srcs/parser/parse_add.cpp\
			srcs/parser/parse_checker.cpp\
			srcs/parser/parse_set_location.cpp\
			srcs/parser/parse_set_server.cpp\
			srcs/parser/parse_tools.cpp\
			srcs/server/init_server.cpp\
			srcs/server/server.cpp\
			srcs/server/main.cpp\
			srcs/utils/tls_tools.cpp\
			srcs/utils/utils_2.cpp\
			srcs/utils/utils_3.cpp\
			srcs/utils/utils.cpp\
			srcs/utils/date.cpp\
			srcs/utils/temp_utils.cpp\
			srcs/utils/client_last_action.cpp\
			srcs/get_next_line/get_next_line.cpp\
			srcs/get_next_line/get_next_line_utils.cpp\
			srcs/request_parsing/save_request.cpp\
			srcs/request_parsing/utils_save_request.cpp\
			srcs/request_parsing/parse_header.cpp\
			srcs/request_parsing/cdtl_headers.cpp\
			srcs/request_handling/request_handling.cpp\
			srcs/request_handling/methods.cpp\
			srcs/request_handling/methods_2.cpp\
			srcs/request_handling/listing.cpp\
			srcs/request_handling/post_put_utils.cpp\
			srcs/request_handling/compression.cpp\
			srcs/request_handling/methods_utils.cpp\
			srcs/request_handling/php_cgi.cpp\
			srcs/request_handling/methods_utils2.cpp\
			srcs/request_handling/listing_tools_generation.cpp\
			srcs/request_handling/listing_tools_html.cpp\
			srcs/error_signal_exit/error.cpp\
			srcs/error_signal_exit/signal.cpp

OBJ = $(SRCS:.cpp=.o)

FLAGS= -Wall -Wextra -Werror
# FLAGS= -Wall -Wextra -Werror -fsanitize=address -g3

CC= clang++

all: $(NAME) 

%.o:%.cpp
	${CC} ${FLAGS} ${INCLUDE} -c $< -o $@

$(NAME): $(OBJ)
	${CC} ${FLAGS} ${OPENSSL} ${OBJ} -o $@ 

clean:
	rm -rf ${OBJ}

fclean: clean
	rm -rf ${NAME}

re: fclean all

.PHONY: re, clean, fclean, all, bonus
