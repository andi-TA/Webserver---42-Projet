#include "Response.hpp"

int	Response::getFdFile( std::string path )
{
    int fd;
	
	if (path.size() > 1 && (path.at(0) == '/'))
		path.erase(0, 1);
    fd = open(path.c_str(), O_RDONLY, 0644);
    if (fd == -1)
	{
        this->buildErrorPage("Not Found", NOT_FOUND, "This resource is not found in server!");
		return (-1);
	}
	return (fd);
}

std::string Response::getContentFile( int fd )
{
    char                buff[SIZE_BUFF];
    ssize_t				size_read = 0;
    std::string         content = "";

    while (true) {
		size_read = read(fd, buff, sizeof(buff));
		if (size_read > 0)
			content.append(buff, size_read);
		else if (size_read == -1)
			throw std::logic_error(INTERNAL_SERVER_ERROR);
		else
			break ;
	}
    return (content);
}

std::string Response::getContentForm( std::string request )
{
	size_t found = request.find("\r\n\r\n");
	if (found != std::string::npos)
		request.erase(0, found + 4);
	return (request);
}

std::string	Response::getResponseInRedirectStatusCode( std::string code )
{
	std::string	code_status[] = {"300", "301", "302", "303", "304", "307", "308", ""};
	std::string message_status[] = {"Multiple Choices", "Moved Permanently", "Found", "See Other", "Not Modified", "Temporary Redirect", "Permanent Redirect", ""};
	int	i;

	i = 0;
	while (!code_status[i].empty())
	{
		if (!code_status[i].compare(code))
			break;
		i++;
	}
	return (message_status[i]);
}

bool	Response::pathIsFull( void )
{
	std::string	check = "";
	std::string root = this->_root;
	std::string	path = this->_path;
	size_t lastChar = this->_root.size() - 1;
	size_t found = std::string::npos;

	if (this->_path.at(0) != '/')
		path = "/" + this->_path;
	if (this->_root.at(lastChar) != '/')
		root = this->_root + "/";

	found = path.find(root);
	if (found != std::string::npos)
	{
		check = path.substr(0, root.size());
		if (!check.compare(root))
			return (true);
	}
	return (false);
}

bool	Response::fullPathWithRoot( void )
{
	size_t	found = this->_path.find(this->_location_by_uri);
	if (found != std::string::npos)
	{
		std::string	val = this->_path.substr(0, this->_location_by_uri.size());
		if (!val.compare(this->_location_by_uri))
		{
			if (val.size() < this->_path.size())
			{
				this->_path = this->_path.substr(val.size(), this->_path.size() - val.size());
				if (!pathIsFull())
					this->_path = this->_root + "/" + this->_path;
				return (this->fileIsGood(this->_path, "", false, true));
			}
		}
	}
	return (false);
}

bool	Response::fileIsGood( std::string uri, std::string bin_path, bool second_parameter_is_binary, bool full_path )
{
	std::string	new_uri = uri;
	
	if (!uri.empty() && uri.at(0) == '/')
		new_uri = "." + uri;

	if (second_parameter_is_binary && !bin_path.empty())
	{
		if (access(bin_path.c_str(), F_OK | R_OK | X_OK) == -1)
		{
			this->buildErrorPage("Bad Gateway", BAD_GATEWAY, "Bad Gateway (error in server web)!");
			return (false);
		}
		return (true);
	}
	if (access(new_uri.c_str(), F_OK | R_OK) == 0 && new_uri.find(this->_root) != std::string::npos)
	{
		return (true);
	}
	else if (!full_path && access(new_uri.c_str(), F_OK) == -1)
	{
		if (this->fullPathWithRoot())
			return (true);
	}
	if (access(new_uri.c_str(), F_OK) == -1)
	{
		this->buildErrorPage("Not Found", NOT_FOUND, "This resource is not found in server!");
	}
	else if (access(new_uri.c_str(), R_OK) == -1)
	{
		this->buildErrorPage("Forbidden", FORBIDDEN, "Access to this resource on the server is denied!");
	}
	else
	{
		this->buildErrorPage("Not Found", NOT_FOUND, "This resource is not found in server!");
	}
	return (false);
}

bool	Response::methodIsImplement( void )
{
	std::string	methods[] = {
		"POST",
		"GET",
		"DELETE",
		""
	};
	int	i = 0;
	while (!methods[i].empty())
	{
		if (!methods[i].compare(this->_method))
			return (true);
		i++;
	}
	this->buildErrorPage("Not Implemented", NOT_IMPLEMENTED, "Method not implemented!");
	return (false);
}

bool	Response::methodeIsAccespt( void )
{
	if (!this->_is_allowed)
	{
		std::string message = "The request method " + this->_method + " is not allowed for the URL " + this->_path;
		this->buildErrorPage("Method Not Allowed", METHOD_NOT_ALLOWED, message);
		return (false);
	}
	else
	{
		if (!this->methodIsImplement())
			return (false);
	}
	return (true);
}

void	Response::sendPageMyErrorPage ( std::string &path, std::string &error_code, std::string &status, std::string &message )
{
	int fd = open(path.c_str(), O_RDONLY, 0644);
	if (fd == -1)
	{
		std::string	str = buildPageWithStatusCode(error_code, status, message);
		this->_body = str;
		this->setResponseType(RESPONSE_ERROR);
		return ;
	}
	FileFD *my_file_fd = new FileFD(fd);
	this->_fd_files.push_back(my_file_fd);
	this->setResponseType(RESPONSE_SIMPLE_HTML_FD);
}

void	Response::buildErrorPage( std::string status, std::string error_code, std::string message )
{
	int			fd = -1;
	std::string	path = this->getErrorIndex(error_code);
	std::string	my_path = "etc/www/errors_page/" + error_code + ".html";

	this->_status = status;
	this->_error_code = error_code;
	
	if (path.empty()) {
		this->sendPageMyErrorPage(my_path, error_code, status, message);
	}
	else
	{
		if ( path.size() > 1 && path.at(0) == '/')
			path.erase(0, 1);
		fd = open(ftAddPointInPath(path).c_str(), O_RDONLY, 0644);
		if (fd != -1 && path.compare("..") && \
			path.compare(".") && path.compare("/") )
		{
			FileFD *my_file_fd = new FileFD(fd);
			this->_fd_files.push_back(my_file_fd);
			this->setResponseType(RESPONSE_SIMPLE_HTML_FD);
		}
		else
			this->sendPageMyErrorPage(my_path, error_code, status, message);
	}
}

std::string	Response::buildPageWithStatusCode( std::string error_code, std::string status, std::string message )
{

	std::string	html = ftBuildHeadHtmlError(error_code);
	std::string	body = "<h1>" + error_code + " " + status + "</h1><p>" + message + "</p>";

	this->_error_code = error_code;
	this->_status = status;
	html.append(body);
	html.append("</body>\n</html>\n");
	return (html);
}

bool	Response::actionIsDelete( void )
{
	std::map<std::string, std::string> elements = this->_request->getKeyValueInRequest();
	for (std::map<std::string, std::string>::iterator it = elements.begin(); it != elements.end(); it++)
	{
		if (!((it->first).compare("delete")) && !((it->second).compare("del")))
			return (true);
	}
	return (false);
}

char**	Response::buildEnvp( void )
{
	std::vector<std::string> envs;
	std::string str_method = "REQUEST_METHOD=" + this->_method;
	std::string filename = "SCRIPT_FILENAME=" + ftGetScriptFilename(this->_path);
	std::string	root = this->_root;
	envs.push_back(str_method);
	envs.push_back(filename);
	envs.push_back(std::string("SERVER_PROTOCOL=HTTP/1.1"));
	envs.push_back(std::string("GATEWAY_INTERFACE=CGI/1.1"));
	envs.push_back(std::string("REDIRECT_STATUS=200"));
	envs.push_back(std::string("HTTP_CONNECTION=close"));
	if (!this->_method.compare("GET"))
	{
		std::string query_string = "QUERY_STRING=" + this->_queryString;
		envs.push_back(query_string);
		envs.push_back(std::string("UPLOAD_DIR=") + this->_upload_dir);
	}
	else if (!this->_method.compare("POST"))
	{
		std::string content_length = "CONTENT_LENGTH=" + this->_request->getPostContentLength();
		std::string content_type = "CONTENT_TYPE=" + this->_request->getPostContentType();
		if ( root.size() > 1 && root.at(0) == '.' )
			root.erase(0, 1);
		std::string	dir_upload = "DEPENDANCE=" + (!(this->actionIsDelete()) ? this->_upload_dir : _root);
		envs.push_back(content_length);
		envs.push_back(content_type);
		envs.push_back(dir_upload);
	}

	const std::map<std::string,std::string>& cookies = this->_request->getCookies();
	if (!cookies.empty()) {
		std::string cookie_header = "HTTP_COOKIE=";
		bool first = true;
		for (std::map<std::string,std::string>::const_iterator it = cookies.begin(); it != cookies.end(); ++it) {
			if (!first) cookie_header += "; ";
			cookie_header += it->first + "=" + it->second;
			first = false;
		}
		envs.push_back(cookie_header);
	}

	char **envp = new char*[envs.size() + 1];
	for (size_t i = 0; i < envs.size(); ++i) {
		envp[i] = new char[envs[i].size() + 1];
		std::strcpy(envp[i], envs[i].c_str());
	}
	envp[envs.size()] = NULL;
	return envp;
}

std::string Response::getPath( void ) {
	return (this->_path);
}

short	Response::getAction( void ) {
	return (this->_cgi_step_stat);
}

void	Response::killPid( void ) {
	if (this->_pid != -1) {
		kill(this->_pid, SIGKILL);
		this->_pid = -1;
	}
}
