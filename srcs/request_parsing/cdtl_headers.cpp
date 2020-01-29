#include "../webserver.hpp"

bool modified_since(const std::string &header, const std::string &file)
{
	std::string date;

	date = get_file_date(file);
    if (compare_date(file, header))
        return (true);
	return (false);
}

bool if_match(const std::string &line, const std::string &file_name)
{
	if (!file_name.compare(""))
		return (false);
	std::vector<t_etag>   etag_vector(ft_parse_line_etag(line));
	for (std::vector<t_etag>::iterator it(etag_vector.begin()); it != etag_vector.end(); it++)
		if (it->hash == ft_sha256(ft_get_features(file_name, it->validation_mod)))
			return (true);
	return (false);
}

std::vector<t_etag>   ft_parse_line_etag(const std::string &line)
{
	size_t i(0);
	t_etag              etag_temp;
	std::vector<t_etag>   etag_vector;
	while (i < line.size())
	{
		etag_temp.hash.clear();
		etag_temp.validation_mod = STRONG_VALIDATION;
		if (line[i] == 'W')
		{
			etag_temp.validation_mod = WEAK_VALIDATION;
			i += 2;
		}
        i++;
		while (line[i] != '"' && line[i])
        {
			etag_temp.hash += line[i];
            i++;
        }
		etag_vector.push_back(etag_temp);
		i += 2;
	}
	return (etag_vector);
}

std::string ft_hexa_sha256(int n)
{
    std::string ret;
    if (n <= 16)
        ret = '0';
    ret += ft_int_to_hex(n, LOWER_CASE);
    return (ret);
}

std::string ft_sha256(const std::string source)
{
	unsigned char		hash[SHA256_DIGEST_LENGTH + 1];
	SHA256_CTX			sha256;
	SHA256_Init(&sha256);
	SHA256_Update(&sha256, source.c_str(), source.size());
	SHA256_Final(hash, &sha256);
	std::string			final_hash;

	for(int i = 0; i < SHA256_DIGEST_LENGTH; i++)
		final_hash += ft_hexa_sha256((int)hash[i]);
	return (final_hash);
}

std::string ft_read_file(const std::string &file_name)
{
	int			fd(0);
	int			ret(0);
	char		buffer[HASH_BUFFER_SIZE + 1];
	std::string source;

	if ((fd = open(file_name.c_str(), O_RDONLY)) < 0)
		error("open", true);
	while ((ret = read(fd, buffer, HASH_BUFFER_SIZE)) > 0)
	{
		buffer[ret] = '\0';
		source += buffer;
	}
	if (close(fd) == -1)
		error("close", true);
	return (source);
}

std::string ft_get_info_file(const std::string &file_name)
{
	struct stat             buf;
	std::string				info_file;
	if (stat(file_name.c_str(), &buf) == -1)
		error("stat", true);

	info_file += ft_int_to_string(buf.st_ino);                     /* File serial number */
	info_file += ft_int_to_string(buf.st_uid);                     /* User ID of the file */
	info_file += ft_int_to_string(buf.st_gid);                     /* Group ID of the file */
	info_file += ft_int_to_string(buf.st_rdev);                        /* Device ID */
	info_file += ft_int_to_string(buf.st_mtimespec.tv_nsec);           /* time of last data modification (nsec)*/
	info_file += ft_int_to_string(buf.st_mtimespec.tv_sec);            /* time of last data modification (sec)*/
	info_file += ft_int_to_string(buf.st_size);                        /* file size, in bytes */
	info_file += ft_int_to_string(buf.st_blocks);                  /* blocks allocated for file */
	info_file += ft_int_to_string(buf.st_blksize);                 /* optimal blocksize for I/O */
	info_file += ft_int_to_string(buf.st_flags);                       /* user defined flags for file */
	info_file += ft_int_to_string(buf.st_gen);                     /* file generation number */
	return (info_file);
}

std::string ft_get_features(const std::string &file_name, const int mod)
{
	std::string features(ft_read_file(file_name));
	switch (mod)
	{
		case STRONG_VALIDATION:
			features += ft_get_info_file(file_name);
			break;
		case WEAK_VALIDATION:
			break;
		default:
			error("hash: validition mod is unknown", false);
	}
	return (features);
}