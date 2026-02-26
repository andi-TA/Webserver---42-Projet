#include "utils.hpp"

bool	ftHttpIsPresent( std::string uri )
{
	std::string	http = "http://";
	std::string http_value = "";
	
	if (uri.size() >= http.size())
	{
		http_value = uri.substr(0, http.size());
		if (!http_value.compare(http))
			return (true);
	}
	return (false);
}

std::string	ftGetContentTypeWithPost( std::string request )
{
	std::stringstream	ss(request);
	std::string	str = "";
	std::string	content = "";

	while (ss >> str)
	{
		if (!str.compare("Content-Type:"))
		{
			ss >> str;
			content = str;
			break;
		}
	}
	ss >> str;
	if (str.find("boundary=") != std::string::npos)
		content += " " + str;
	return (content);
}

void	ftFreeTab( char ***tab )
{
	int	i;

	i = 0;
	while (tab[i] != NULL)
	{
		delete tab[i];
		i++;
	}
	delete tab;
}

std::string	ftGetScriptFilename( std::string uri )
{
	size_t found = uri.find("?");
	if (found != std::string::npos)
		uri.erase(found, uri.size() - found);
	size_t	pos = uri.find_last_of("/");
	if (pos != std::string::npos)
		uri = uri.erase(0, pos + 1);
	return (uri);
}

std::string ftGetExtensionInFile( std::string file )
{
    size_t found = -1;

    found = file.rfind(".");
    if (found != std::string::npos)
        file.erase(0, found);
    return (file);
}

std::string	ftGetSubContent( std::string & body )
{
	std::string	str = "";

	str = body.substr(0, BUFSIZ);
	body.erase(0, str.size());
	return (str);
}

std::string	ftBuildHeadHtmlDirList( void )
{
	std::string	html;

	html = "<!DOCTYPE html>\n"
	"<html lang=\"en\">\n"
	"<head>\n"
    "<meta charset=\"UTF-8\">\n"
    "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n"
    "<title>DirList</title>\n"
	"<link rel='stylesheet' href='/styles/dirlist.css'>"
	"</head>\n"
	"<body>\n"
	"<h1> Dir-list </h1>\n"
	" <table><thead><tr><th>Name file</th><th>Last modified</th><th>File size</th></tr></thead><tbody>";
	return (html);
}

std::string	ftGetLastModifyFile( std::string file )
{
	struct stat	modif_file = {};
	std::string	result = "-";

	if (stat(ftAddPointInPath(file).c_str(), &modif_file) == 0)
		result = ctime(&modif_file.st_mtime);
	return (result);
}

std::string	ftGetSizeFile( std::string file )
{
	std::stringstream	ss;
	std::string			result = "";
	struct stat			modif_file = {};
	double				size_file = 0;

	if (stat(ftAddPointInPath(file).c_str(), &modif_file) == 0)
	{
		size_file = modif_file.st_size;
		size_file /= 1000;
	}
	ss << size_file;
	result = ss.str() + "K";
	return (result);
}

std::string	ftBuildHeadHtmlError( std::string error_code )
{
	std::string	html;

	html = "<!DOCTYPE html>\n"
	"<html lang=\"en\">\n"
	"<head>\n"
    "<meta charset=\"UTF-8\">\n"
    "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n"
    "<title> status " + error_code + " </title>\n"
	"</head>\n"
	"<body>\n";
	return (html);
}

std::string	ftAddPointInPath( std::string path )
{
	if (!path.empty() && path.size() > 1 && path.at(0) != '.' && path.at(0) == '/')
		return ("." + path);
	return (path);
}

std::string	ftGetDirInPath( std::string &path )
{
	size_t	found = path.find_last_of("/");
	std::string	dir = path;

	if ( found != std::string::npos )
		dir = path.substr(0, found);
	return (dir);
}

bool	ftUriIsDir( std::string uri, std::string root )
{
	struct stat file_info = {};

	if ( uri.empty() || root.empty() )
		return (false);

	std::string path_uri = uri;
	if ( uri[0] != '.' )
		path_uri = "." + uri;
	std::string	full_path_uri = root + uri;
	if ( full_path_uri[0] != '.' )
		full_path_uri = "." + full_path_uri;

	if (stat(ftAddPointInPath(path_uri).c_str(), &file_info) == 0 && uri.find(root) != std::string::npos)
	{
		if (file_info.st_mode & S_IFDIR)
		{
			DIR *dir = opendir(path_uri.c_str());
			if (dir == NULL)
				return (false);
			closedir(dir);
			return (true);
		}
	}

	if (stat(ftAddPointInPath(full_path_uri).c_str(), &file_info) == 0)
	{
		if (file_info.st_mode & S_IFDIR)
		{
			DIR *dir = opendir(full_path_uri.c_str());
			if (dir == NULL)
				return (false);
			closedir(dir);
			return (true);
		}
	}
	return (false);
}

int	ftGetLastChar( std::string str )
{
	int	pos = str.size() - 1;

	if (!str.empty())
		return (str.at(pos));
	return (-1);
}