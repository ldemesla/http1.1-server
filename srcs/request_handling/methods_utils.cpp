#include "../webserver.hpp"

std::string					get_content_type(std::string file)
{
	std::string res;
	int i;
	int n;
	int dot = 0;
	struct stat info;

	stat(file.c_str(), &info);
	std::string ext[67] = {"php", "aac", "abw", "arc", "avi", "azw", "bin", "bz", "bz2", "csh", "css", "csv", "doc", "docsx", "eot", "epub", "gif", "htm", "html", "ico",
	"ics", "jar", "jpeg", "jpg", "js", "json", "mid", "midi", "mpeg", "mpkg", "odp", "ods", "odt", "oga", "ogv", "ogx", "otf", "png", "pdf", "ppt", "pptx", "rar", "rtf", "sh"
	"svg", "swf", "tar", "tif", "tiff", "ts", "ttf", "vsd", "wav", "weba", "webm", "webp", "woff" ,"woff2", "xhtml", "xls", "xlsx","xml", "xul", "zip", "3gp", "3g2", "7z"};
	std::string ret[67] = {"text/html", "audio/aac", "application/x-abiword", "application/octet-stream", "video/x-msvideo", "application/vnd.amazon.ebook", "application/octet-stream", "application/x-bzip",
	"application/x-bzip2", "application/x-csh", "text/css", "text/csv", "application/msword", "application/vnd.openxmlformats-officedocument.wordprocessingml.document", "application/vnd.ms-fontobject",
	"application/epub+zip", "image/gif", "text/html", "text/html", "image/x-icon", "text/calendar", "application/java-archive", "image/jpeg", "image/jpeg" ,"application/javascript", "application/json",
	"audio/midi","audio/midi", "video/mpeg", "	application/vnd.apple.installer+xml", "application/vnd.oasis.opendocument.presentation", "application/vnd.oasis.opendocument.spreadsheet", "application/vnd.oasis.opendocument.text", "audio/ogg", "video/ogg", "application/ogg", "font/otf", "image/png", "application/pdf", "application/vnd.ms-powerpoint", "application/vnd.openxmlformats-officedocument.presentationml.presentation",
	"application/x-rar-compressed", "application/rtf", "application/x-sh", "image/svg+xml", "application/x-shockwave-flash", "application/x-tar", "image/tiff", "image/tiff", "application/typescript", "font/ttf", "application/vnd.visio", "audio/x-wav", "audio/webm", "video/webm", "image/webp", "font/woff", "font/woff2", "application/xhtml+xml", "application/vnd.ms-excel", "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet",
	"application/xml", "application/vnd.mozilla.xul+xml", "application/zip", "video/3gpp", "video/3gpp2", "application/x-7z-compressed"};

	if (!file.compare(".") || info.st_mode & S_IFDIR)
		return ("text/html");
	i = 0;
	while (file[i])
		if (file[i++] == '.')
			dot++;
	i = 0;
	while (file[i])
	{
		if (file[i++] == '.')
			dot--;
		if (!dot)
			break ;
	}
	if (!file[i])
		return ("text/plain");
	file = file.substr(i, file.size());
	i = 0;
	while (i < 66)
	{
		if (!(n = file.compare(ext[i])))
		{
			return (ret[i]);
		}
		i++;
	}
	return ("text/plain");
}

std::string generate_200_header(int gzip, std::string file, unsigned long content_len)
{
	std::string features;

	features = ft_get_features(file, 1);
    std::string header("HTTP/1.1 200 ok\r\nContent-Language: en-US\r\nDate: " + get_date() +"\r\nServer: Webserver/1.0\r\nContent-Length: "
	+ ft_long_to_string(content_len) + "\r\nAccept-Encoding: gzip, chunked\r\nEtag: \"" + ft_sha256(features) +"\"\r\nContent-Type: " +  get_content_type(file) + "\r\n");
	if (gzip)
        header += "Content-Encoding: gzip\r\n";
	header += "\r\n";
    return (header);
}

std::string get_file_name(std::string file)
{
	int i;

	i = file.size() - 1;
	while (i > 0 && file[i] != '/')
		i--;
	file = file.substr(i, file.size());
	return (file);
}