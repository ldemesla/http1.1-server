#include "../webserver.hpp"

std::string get_file_date(const std::string &file)
{
	struct stat st;
	std::string sec;
	struct tm tm;
	std::string date;
	char buffer[BUFFER_SIZE + 1];

	stat(file.c_str(), &st);
	sec = ft_long_to_string(st.st_mtime);
	strptime(sec.c_str(), "%s", &tm);
	strftime(buffer, sizeof(buffer), "%a, %d %b %Y %H:%M:%S GMT", &tm);
	date = buffer;
	return (date);
}

std::string get_date()
{
    struct timeval tv;
    struct tm tm;
    struct timezone tz;
    char buffer[BUFFER_SIZE + 1];
    std::string date;

    gettimeofday(&tv, &tz);
    strptime(ft_long_to_string(tv.tv_sec).c_str(), "%s", &tm);
    strftime(buffer, sizeof(buffer), "%a, %d %b %Y %H:%M:%S GMT", &tm);
    date = buffer;
	return (date);
}
//Compare if date1 is more recent than date2
bool compare_date(std::string date1, std::string date2)
{
	struct tm one;
	struct tm two;

	strptime(date1.c_str(), "%a, %d %b %Y %H:%M:%S GMT", &one);
	strptime(date2.c_str(), "%a, %d %b %Y %H:%M:%S GMT", &two);
	if (one.tm_year > two.tm_year)
		return (true);
	else if (one.tm_year == two.tm_year && one.tm_yday > two.tm_yday)
		return (true);
	else if (one.tm_year == two.tm_year && one.tm_yday == two.tm_yday && one.tm_hour
	> two.tm_hour)
		return (true);
	else if (one.tm_year == two.tm_year && one.tm_yday == two.tm_yday && one.tm_hour
	== two.tm_hour && one.tm_min > two.tm_min)
		return (true);
	else if (one.tm_year == two.tm_year && one.tm_yday == two.tm_yday && one.tm_hour
			== two.tm_hour && one.tm_min == two.tm_min && one.tm_sec >= two.tm_sec)
		return (true);
	return (false);
}