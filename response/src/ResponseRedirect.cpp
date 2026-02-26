#include "Response.hpp"

bool	Response::redirectExternalHttp( void )
{
	std::string status = this->getResponseInRedirectStatusCode(this->_redirect_uri_code);
	std::string	header = "HTTP/1.0 " + this->_redirect_uri_code + " " + status + "\r\n"
		"Location: " + this->_redirect_uri + "\r\n"
		"Content-Length: 0\r\n"
		"Connection: close\r\n"
		"\r\n";

	if (send(this->_fd_client, header.c_str(), header.size(), MSG_NOSIGNAL) == -1)
		throw std::logic_error("Error: on body response sending\n");
	return (true);
}

void	Response::checkIfDirOrNot( void )
{
	this->_path = getRealPathWithUri();
	if (ftUriIsDir(this->_path, this->_root))
	{
		std::string uri = (ftGetLastChar(this->_path) == '/' || this->_index.at(0) == '/' ) ? this->_path + this->_index : \
			this->_path + "/" + this->_index;
		if (!fileIsGood(uri, "", false, true))
			return ;
		this->_path = uri;
	}
	else
	{
		if (rootRequestMatchWithConfigRoot())
		{
			if (!fileIsGood(this->_path, "", false, false))
				return ;
		}
		else
		{
			this->_path = (!this->_path.compare(this->_location_by_uri)) ? this->_root : this->_path;
			if (!fileIsGood(this->_path, "", false, false))
			{
				return ;
			}
		}
	}
}

void	Response::sendResponseWithoutDirList(void)
{
	if (this->_is_allowed)
	{
		if (!this->_path.compare(this->_location_by_uri) && !this->_auto_index)
		{
			if (this->_root.empty() || this->_index.empty())
			{
				this->buildErrorPage("Not Found", NOT_FOUND, "This resource is not found in server!");
				return ;
			}
			if (ftGetLastChar(this->_root) == '/')
				this->_path = this->_root + this->_index;
			else
				this->_path = this->_root + "/" + this->_index;
		}
		else
			this->checkIfDirOrNot();
		if ((!this->_method.compare("POST")) || !this->_method.compare("GET"))
		{
			this->sendReponseInRequestFile();
		}
		else
		{
			this->buildErrorPage("Bad Request", BAD_REQUEST, "Bad Request!");
		}
	}
	else
	{
		std::string	message = "The request method " + this->_method + " is not allowed for the URL " + this->_path;
		this->buildErrorPage("Method Not Allowed", METHOD_NOT_ALLOWED, message);
	}
}

std::string	Response::getRealPathWithUri( void )
{
	std::string	uri = (!this->_path.compare(this->_location_by_uri)) ? this->_root : this->_path;
	size_t		found;

	found = this->_path.find(this->_location_by_uri);
	if (found != std::string::npos)
	{
		std::string	val = this->_path.substr(0, this->_location_by_uri.size());
		if (!val.compare(this->_location_by_uri))
		{
			if (val.size() < this->_path.size())
			{
				uri = this->_path.substr(val.size(), this->_path.size() - val.size());
				uri = (ftGetLastChar(this->_root) == '/' || uri.at(0) == '/') ? this->_root + uri \
					: this->_root + "/" + uri;
			}
		}
	}
	return (uri);
}

std::string	Response::getRealPathWitUri( std::string& uri )
{
	std::string	res = "-";
	size_t	found = uri.find(this->_location_by_uri);

	if (found != std::string::npos)
	{
		std::string	val = this->_path.substr(0, this->_location_by_uri.size());
		if (!val.compare(this->_location_by_uri))
		{
			if (val.size() <= this->_path.size())
				res = this->_root + "/" + this->_path.substr(val.size(), this->_path.size() - val.size());
		}
	}
	return (res);
}

void	Response::dirList( void )
{
	s_stat		file_info = {};
	std::string	path = this->getRealPathWithUri();

	if (stat(ftAddPointInPath(path).c_str(), &file_info) == -1)
	{
		this->buildErrorPage("Not Found", NOT_FOUND, "This resource is not found in server!");
	}
	if (S_ISREG(file_info.st_mode))
	{
		this->_path = path;
		this->sendReponseInRequestFile();
	}
	else if (S_ISDIR(file_info.st_mode))
	{
		std::string	html 			= "";
		std::string val				= "";
		std::string	dir_path		= "";
		std::string last_modif		= "-";
		s_dirent	*dir_content	= NULL;
		DIR	*dir					= opendir(ftAddPointInPath(path).c_str());
	
		if (dir == NULL)
		{
			this->buildErrorPage("Forbidden", FORBIDDEN, "Access to this resource on the server is denied!");
			return ;
		}
		this->_content_type = "text/html";
		html = ftBuildHeadHtmlDirList();
		dir_content = readdir(dir);
		while (dir_content)
		{
			int	index_end_path = static_cast<int>((this->_path).size()) - 1;
			if (index_end_path < static_cast<int>(this->_path.size()))
			{
				dir_path = ((this->_path).at(index_end_path) == '/') ? (this->_path + dir_content->d_name) : (this->_path + "/" + dir_content->d_name);
				html.append("<tr><td class='date'><a href=\"" + dir_path + "\">" + dir_content->d_name + "</a></td>");
				val = dir_content->d_name;
				if (val.compare(".") && val.compare(".."))
				{
					val = dir_path + "/" + val;
					last_modif = ftGetLastModifyFile(this->getRealPathWitUri(val) + "/" + dir_content->d_name);
				}
				html.append("<td class='date'>" + last_modif + "</td>" );
				html.append("<td class='date'>" + ftGetSizeFile(this->getRealPathWitUri(val) + "/" + dir_content->d_name) + "</td></tr>");
			}
			dir_content = readdir(dir);
		}
		html.append("</tbody></table></body>\n</html>\n");
		this->setResponseType(RESPONSE_DIRLIST);
		closedir(dir);
		this->_body = html;
	}
}

void	Response::redirectResponse( void )
{
	this->setResponseType(RESPONSE_REDIRECT);
}


bool	Response::loopRedirectionDetected( std::string location_sub )
{
	size_t	found = location_sub.find(this->_location_by_uri);

	if (found != std::string::npos)
	{
		location_sub.erase(0, found);
		if (!this->_location_by_uri.compare(location_sub))
			return (true);
	}
	return (false);
}

bool	Response::moreVerificationRedirectIsTrue( void )
{
	if (!redirectionIsOkWithUri(this->_redirect_uri))
		return (false);
	this->redirectResponse();
	return (true);
}

bool	Response::redirectionIsOkWithUri( std::string uri )
{
	std::string	location_by_uri = "";
	if (uri.size() < this->_redirect_uri.size())
	{
		size_t	lenght_build_uri = uri.size() - 1;
		location_by_uri = this->_redirect_uri.substr(lenght_build_uri, this->_redirect_uri.size() - lenght_build_uri);
		if (!location_by_uri.compare(this->_location_by_uri) || loopRedirectionDetected(location_by_uri))
		{
			this->buildErrorPage("Loop detected", LOOP_REDIRECT, "LOOP REDIRECT DETECTED");
			return (false);
		}
	}
	this->redirectResponse();
	return (true);
}

bool	Response::redirectionIsGood( void )
{
	if (ftHttpIsPresent(this->_redirect_uri))
	{
		std::string	build_uri = "http://" + this->_server_name + ":" + this->_server_port + "/";
		return(this->redirectionIsOkWithUri(build_uri));
	}
	return(this->moreVerificationRedirectIsTrue());
}