#include "../webserver.hpp"

void		*ft_bzero(void *ptr, size_t len)
{
	for (size_t i(0); i < len; i++)
		((char *)ptr)[i] = 0;
	return (ptr);
}

int    ft_compress_gzip(int fd_input, int fd_output)
{
    int             ret, flush;
    unsigned        have;
    z_stream        strm;
    unsigned char   buffer[COMPRESSED_BUFFER_SIZE + 1];
    unsigned char   out[COMPRESSED_BUFFER_SIZE + 1];

    /* init */
    strm.zalloc = Z_NULL;   
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    if (deflateInit2(&strm,
				Z_BEST_COMPRESSION,
				Z_DEFLATED,
				MOD_GZIP_ZLIB_WINDOWSIZE + 16, 
				MOD_GZIP_ZLIB_CFACTOR,
				Z_DEFAULT_STRATEGY) != Z_OK) 
		error("deflateInit2", true);
    /* start deflate */
    do {
        strm.avail_in = read(fd_input, buffer, COMPRESSED_BUFFER_SIZE);
        flush = strm.avail_in == 0 ? Z_FINISH : Z_NO_FLUSH;
        strm.next_in = buffer;
        do {strm.avail_out = COMPRESSED_BUFFER_SIZE;
            strm.next_out = out;
            ret = deflate(&strm, flush);
            have = COMPRESSED_BUFFER_SIZE - strm.avail_out;
            if ((ret = write(fd_output, out, have)) != (int)have)
            {
                (void)deflateEnd(&strm);
                return Z_ERRNO;
            }
        } while (strm.avail_out == 0);
    } while (flush != Z_FINISH);
    (void)deflateEnd(&strm);
    return Z_OK;
}

void compressed_file_fd(t_client &client, std::string file)
{
    int fd;
    std::string res;
    struct stat info;

    if (file.compare(client.request.file) && stat_temp(client, &info, file) == -1)
    {
        client.read_fd = 0;
        return ;
    }
    else
        stat(file.c_str(), &info);
    client.read_fd = open_temp_file(client, client.temp_file, O_RDWR | O_CREAT | O_TRUNC, 1);
    if (file.compare(client.request.file))
        fd = open_temp_file(client, file, O_RDONLY, 1);
    else
        fd = open(file.c_str(), O_RDONLY);
    ft_compress_gzip(fd, client.read_fd);
	close(fd);
    close(client.read_fd);
    client.read_fd = open_temp_file(client, client.temp_file, O_RDONLY, 0);
    stat_temp(client, &info ,client.temp_file);
    client.request.file_size = info.st_size;
    client.request.res = generate_200_header(1, client.request.file, client.request.file_size);
    if (file.compare(client.request.file))
        delete_temp_file(client, file);
}

void	ft_inflate_file_fd(const std::string fileName)
{
	int				ret;
	char 			unzipBuffer[DECOMPRESSED_BUFFER_SIZE + 1];
	unsigned int	unzippedBytes;
	std::string		unzippedData;
	int				fd;
	int				temp_fd;
	char 			buffer[DECOMPRESSED_BUFFER_SIZE + 1];
	gzFile			inFileZ = gzopen((char *)fileName.c_str(), "rb");
	
	if (inFileZ == NULL)
		return ;	
	if ((temp_fd = open((fileName + ".gz_tmp").c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0666)) <= 0)
		std::cerr << "error: open()" << std::endl;
	while ((unzippedBytes = gzread(inFileZ, unzipBuffer, DECOMPRESSED_BUFFER_SIZE)) > 0)
			write(temp_fd, unzipBuffer, unzippedBytes);
	gzclose(inFileZ);
	close(temp_fd);
	if ((temp_fd = open((fileName + ".gz_tmp").c_str(), O_RDONLY)) <= 0)
	{
		perror("openopen");
		std::cerr << "error: open()" << std::endl;
	}
	if ((fd = open(fileName.c_str(), O_WRONLY | O_TRUNC, 0666)) <= 0)
		std::cerr << "error: open()" << std::endl;
	while ((ret = read(temp_fd, buffer, DECOMPRESSED_BUFFER_SIZE)) > 0)
		write(fd, buffer, ret);
	unlink((fileName + ".gz_tmp").c_str());
	close(temp_fd);
	close(fd);
}

std::string	ft_inflate_file(const std::string fileName)
{
	char 			unzipBuffer[DECOMPRESSED_BUFFER_SIZE + 1];
	unsigned int	unzippedBytes;
	std::string		unzippedData;
	gzFile			inFileZ = gzopen((char *)fileName.c_str(), "rb");
	
	if (inFileZ == NULL)
		std::cerr << "error: gzopen()" << std::endl;
	while ((unzippedBytes = gzread(inFileZ, unzipBuffer, DECOMPRESSED_BUFFER_SIZE)) > 0)
		unzippedData.append(unzipBuffer, unzippedBytes);
	unlink(fileName.c_str());
	gzclose(inFileZ);
	return (unzippedData);
}