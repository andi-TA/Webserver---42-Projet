#include "Response.hpp"
#include "ResponseCgi.cpp"
#include "ResponseRedirect.cpp"
#include "ResponseUtils.cpp"
#include "../../server/include/ServerFD.hpp"

Response::Response( void ) { 
	this->_pid = -1;
	this->_type_response_to_send = RESPONSE_ERROR;
	this->_cgi_step_stat = CGI_STEP_ONE;
	this->_header_sent = false;
	this->_get_body = false;
}

Response::Response( const Response& cpy )
{
	(*this) = cpy;
}

Response::~Response( void )
{
	this->killPid();
}

Response &	Response::operator=( const Response& cpy )
{
	this->_header_sent = cpy._header_sent;
	this->_pid = cpy._pid;
	this->_cgi_step_stat = cpy._cgi_step_stat;
	this->_cgi_ext = cpy._cgi_ext;
	this->_tmp = cpy._tmp;
	this->_get_body = cpy._get_body;
	this->_body = cpy._body;
	this->_request = cpy._request;
	this->_method = cpy._method;
	this->_path = cpy._path;
	this->_location_by_uri = cpy._location_by_uri;
	this->_content_type = cpy._content_type;
	this->_fd_client = cpy._fd_client;
	this->_cgi = cpy._cgi;
	this->_root = cpy._root;
	this->_auto_index = cpy._auto_index;
	this->_support_cookies = cpy._support_cookies;
	this->_index = cpy._index;
	this->_queryString = cpy._queryString;
	this->_is_allowed = cpy._is_allowed;
	this->_redirect_uri = cpy._redirect_uri;
	this->_server_name = cpy._server_name;
	this->_server_port = cpy._server_port;
	this->_redirect_uri_code = cpy._redirect_uri_code;
	this->_type_response_to_send = cpy._type_response_to_send;
	this->_request_error_code = cpy._request_error_code ;
	this->_request_message_error = cpy._request_message_error ;
	this->_request_have_error = cpy._request_have_error ;
	this->_status = cpy._status;
	this->_error_code = cpy._error_code;
	this->_error_index = cpy._error_index;
	this->_upload_dir = cpy._upload_dir;
	this->_server_fd = cpy._server_fd;
	this->_response_body = cpy._response_body;
	return (*this);
}

Response::Response( int server_fd, int client_fd, IPollFD& allFD, Request& request )
{
	this->_pid = -1;
	this->_server_fd = server_fd;
	this->_get_body = false;
	this->_body = "";
	this->_tmp = "";
	this->_cgi_ext = "";
	this->_request = &request;
	this->_method = request.getMethod();
	this->_path = request.getPath();
	this->_upload_dir = allFD.getServer(server_fd)->getSimpleUploadDir(this->_path);
	this->_location_by_uri = allFD.getServer(server_fd)->getLocationUri(this->_path);
	this->_content_type = request.getContentType();
	this->_fd_client = client_fd;
	this->_cgi_ext = ftGetExtensionInFile(this->_path);
	this->_cgi = allFD.getServer(server_fd)->getCgiPass(request.getPath(), this->_cgi_ext);
	this->_root = allFD.getServer(server_fd)->getRoot(request.getPath());
	this->_auto_index = allFD.getServer(server_fd)->autoIndexON(request.getPath());
	this->_support_cookies = allFD.getServer(server_fd)->cookiesSupportON(request.getPath());
	this->_index = allFD.getServer(server_fd)->getIndex(request.getPath());
	this->_queryString = request.getQueryString();
	this->_is_allowed = allFD.getServer(server_fd)->isAllowed(this->_path, this->_method);
	this->_redirect_uri = allFD.getServer(server_fd)->getRedirectPath(this->_path);
	this->_server_name = allFD.getServer(server_fd)->getHost();
	this->_server_port = allFD.getServer(server_fd)->getPort();
	this->_type_response_to_send = RESPONSE_ERROR;
	this->_redirect_uri_code = allFD.getServer(server_fd)->getRedirectCode(this->_path);
	this->_request_have_error = request.isRequestError();
	this->_request_error_code = request.getCodeError();
	this->_request_message_error = request.getMessageError();
	this->_status = "OK";
	this->_error_code = OK;
	this->_error_index = allFD.getServer(server_fd)->getErrorIndex();
	this->_header_sent = false;
	this->_response_body = "";

}

void	Response::setResponseType( short type )
{
	this->_type_response_to_send = type;
}

std::string	Response::getErrorIndex( std::string error_code )
{
	try
	{
		return (this->_error_index[error_code]);
	}
	catch(const std::exception& e)
	{
		return ("");
	}
	return ("");
}


short	Response::getResponseType( void )
{
	return (this->_type_response_to_send);
}

void	Response::sendBody( const std::string &body ) {
	if (send(this->_fd_client, body.c_str(), body.size(), MSG_NOSIGNAL) == -1)
		throw std::logic_error("Error: on body response sending\n");
}

pid_t	Response::getChildPid( void )  
{
	return (this->_pid);
}

void Response::addSetCookie( const std::string& cookie_header_value ) {
	this->_set_cookies.push_back( cookie_header_value );
}

void	Response::sendHeader( std::string &body ) {
	if (this->_header_sent)
		return ;

	this->_header_sent = true;
	std::ostringstream	oss;

	oss << body.size();
	if (this->_error_code.compare(OK) || (!this->_cgi.empty() \
		&& !ftGetExtensionInFile(ftGetScriptFilename(this->_path)).compare(".py")))
		this->_content_type = "text/html";

	std::string	header = "HTTP/1.0 " + this->_error_code + " " + this->_status + "\r\n"
			"Content-type: " + this->_content_type + "\r\n"
			"Content-length: " + oss.str() + "\r\n"
			"Connection: close\r\n";

	if ( this->_support_cookies ) {
		for (std::vector<std::string>::iterator it = this->_set_cookies.begin(); it != this->_set_cookies.end(); ++it) {
			header += std::string("Set-Cookie: ") + *it + "\r\n";
		}
	}

	header += "\r\n";

	if (send(this->_fd_client, header.c_str(), header.size(), MSG_NOSIGNAL) == -1)
		throw std::logic_error("Error: on body response sending\n");
}

void	Response::sendResponse( std::string &body )
{
	std::ostringstream	oss;

	oss << body.size();
	if (this->_error_code.compare(OK))
		this->_content_type = "text/html";

	std::string	header = "HTTP/1.0 " + this->_error_code + " " + this->_status + "\r\n"
			"Content-type: " + this->_content_type + "\r\n"
			"Content-length: " + oss.str() + "\r\n"
			"Connection: close\r\n"
			"\r\n";

	if (send(this->_fd_client, header.c_str(), header.size(), MSG_NOSIGNAL) == -1)
		throw std::logic_error("Error: on header response sending\n");
	if (send(this->_fd_client, body.c_str(), body.size(), MSG_NOSIGNAL) == -1)
		throw std::logic_error("Error: on body response sending\n");
}

void	Response::sendReponseInRequestFile( void )
{
	if (!fileIsGood(this->_path, "", false, false))
		return ;
	this->_cgi = this->_request->getServer()->getCgiPass(this->_request->getPath(), ftGetExtensionInFile(this->_path));
	if (this->_cgi.empty())
	{
		int	fd = this->getFdFile(this->_path);
		this->setResponseType(RESPONSE_SIMPLE_HTML_FD);

		FileFD	*my_file_fd = new FileFD(fd);
		this->_fd_files.push_back(my_file_fd);
	}
	else if (!this->_cgi.empty() && fileIsGood("", this->_cgi, true, false))
	{
		this->setFdPipeWithCgi();
		this->setResponseType(RESPONSE_WITH_CGI);
		this->_cgi_step_stat = CGI_STEP_ONE;
		this->_pid = -1;
	}
}

void	Response::sendResponseWithMethodDelete( void )
{
	if (this->_is_allowed)
	{
		if (!fileIsGood(this->_path, "", false, false))
			return;
		if (!this->_cgi.empty())
		{

			this->setResponseType(RESPONSE_WITH_CGI);
		}
		else
		{
			
			this->setResponseType(RESPONSE_SIMPLE_HTML_FD);
		}
	}
	else
	{
		std::string message = "The request method " + this->_method + " is not allowed for the URL " + this->_path;
		this->buildErrorPage("Method Not Allowed", METHOD_NOT_ALLOWED, message);
	}
}	

void	Response::createFilesResponse( void )
{
	if (!checkRequestIfGood() || \
		this->getResponseType() == RESPONSE_UPLOAD_WITHOUT_CGI) {
		return ;
	}

	if (!this->_redirect_uri.empty())
	{
		if (!this->redirectionIsGood())
			return ;
	}
	else if (this->_auto_index)
	{
		this->dirList();
	}	
	else if (!this->_auto_index)
	{
		if (!this->_is_allowed)
		{
			std::string	message = "The request method " + this->_method + " is not allowed for the URL " + this->_path;
			this->buildErrorPage("Method Not Allowed", METHOD_NOT_ALLOWED, message);
			return ;
		}
		this->sendResponseWithoutDirList();
	}
}

bool	Response::sendResponseSimpleFD( int client_fd, IPollFD& poll_event ) {
	short	revent = poll_event.getFdRevent(this->_fd_files[0]->getFD());
	if ((revent & POLLIN) && !this->_get_body)
	{
		this->_body = this->getContentFile(this->_fd_files[0]->getFD());
		this->_get_body = true;
		poll_event.setFdEvent(client_fd, POLLOUT);
	}
	else if ((revent & POLLHUP) || (revent & POLLERR) || (revent & POLLNVAL))
		throw std::logic_error(INTERNAL_SERVER_ERROR);
	
	if (!this->_get_body)
		return (false);

	if (!(poll_event.getFdRevent(client_fd) & POLLOUT))
		return (false);

	this->sendHeader(this->_body);

	if (this->_body.empty()) {
		return (true);
	}

	this->_tmp = this->_body.substr(0, BUFSIZZ	);
	this->sendBody(this->_tmp);
	this->_body.erase(0, this->_tmp.size());
	if (this->_body.empty()) {
		return (true);
	}
	return (false);
}

bool	Response::sendResponseSimpleUpload( IPollFD& poll_event ) {	
	for (std::vector<FileFD *>::iterator it = this->_request->getUploadFileFD().begin(); \
		it != this->_request->getUploadFileFD().end(); ++it) {
		short	revent = poll_event.getFdRevent((*it)->getFD());
		if ((revent & POLLOUT) && !(*it)->shouldClose()) {
			if (!(*it)->writeContent())
				continue;
			(*it)->setCloseYes();
		}
		else if ((revent & POLLHUP) || (revent & POLLERR) || (revent & POLLNVAL))
			throw std::logic_error(INTERNAL_SERVER_ERROR);
	}
	for (std::vector<FileFD *>::iterator it = this->_request->getUploadFileFD().begin(); \
		it != this->_request->getUploadFileFD().end(); ++it) {
		if (!(*it)->shouldClose())
			return (false);
	}
	this->setResponseType(RESPONSE_SIMPLE_HTML_FD);
	return (false);
}

bool	Response::SendResponseStr( IPollFD& poll_event )
{
	short	revent = poll_event.getFdRevent(this->_fd_client);

	if ((revent & POLLHUP) || (revent & POLLERR) || (revent & POLLNVAL))
		throw std::logic_error(INTERNAL_SERVER_ERROR);
	if (!(revent & POLLOUT)) {
		poll_event.setFdEvent(this->_fd_client, POLLOUT);
		return (false);
	}
	this->sendHeader(this->_body);
	this->sendBody(this->_body);
	return (true);
}

bool	Response::sendResponse( int client_fd, short type, IPollFD& poll_event ) {				
	switch (type)
	{
		case RESPONSE_SIMPLE_HTML_FD:
			return (this->sendResponseSimpleFD(client_fd, poll_event));

		case RESPONSE_UPLOAD_WITHOUT_CGI:
			return (this->sendResponseSimpleUpload(poll_event));

		case RESPONSE_WITH_CGI:
			return (this->sendResponseWithCgi(poll_event));

		case RESPONSE_ERROR:
			return (this->SendResponseStr(poll_event));

		case RESPONSE_REDIRECT:
			return (this->redirectExternalHttp());

		case RESPONSE_DIRLIST:
			return (this->SendResponseStr(poll_event));

	}
	return (false);
}