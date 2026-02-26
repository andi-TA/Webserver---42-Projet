#include "../include/ClientFD.hpp"
#include "../include/SessionManager.hpp"


/**
 * ********************** Coplien *******************
 */

ClientFD::ClientFD( void ) {
	
	//	default constructor
	this->_type = "client";
	this->_fd = 0;
	this->_error_code = OK;
	this->_event = POLLIN | POLLOUT | POLLERR | POLLHUP | POLLNVAL;
	this->_server_fd = 0;
	this->_should_close = false;
	this->_action = NONE;
	this->_last_time_activity = std::time(NULL);
	this->_content_length = 0;
}

ClientFD::ClientFD( int const& client_fd, int const& server_fd ){

	//	constructor whit fd
	this->_last_time_activity = std::time(NULL);
	this->_type = "client";
	this->_fd = client_fd;
	this->_error_code = OK;
	this->_event = POLLIN | POLLOUT | POLLERR | POLLHUP | POLLNVAL;
	this->_server_fd = server_fd;
	this->_should_close = false;
	this->_action = NONE;
	this->_content_length = 0;
}

ClientFD::ClientFD( const ClientFD& cpy ) {
	
	//	copy constructor
	(*this) = cpy;
}

ClientFD::~ClientFD( void ) {
	for (size_t i = 0; i < this->_request.getUploadFileFD().size(); i++) {
		delete (this->_request.getUploadFileFD()[i]);
	}
	this->_request.getUploadFileFD().clear();
	for (size_t i = 0; i < this->_response._fd_files.size(); i++) {
		delete (this->_response._fd_files[i]);
	}
	this->_response._fd_files.clear();
	WebServer::closeFD(this->_fd);
}

ClientFD&	ClientFD::operator=( const ClientFD& cpy ) {
	
	
	//	copy assignement
	this->_action = cpy._action;
	this->_type = cpy._type;
	this->_fd = cpy._fd;
	this->_event = cpy._event;
	this->_server_fd = cpy._server_fd;
	this->_content_length = cpy._content_length;
	this->_should_close = cpy._should_close;
	this->_error_code = cpy._error_code;
	return (*this);
}







/**
 * ********************* PUBLIC METHODS ***********************
 */


void	ClientFD::updateLastTimeActivity( void ){
	this->_last_time_activity = std::time(NULL);
}

void	ClientFD::deleteFilesResponse( IPollFD& poll_event ) {
	for (size_t i = 0; i < this->_request.getUploadFileFD().size(); i++) {
		poll_event.deletePollFD(this->_request.getUploadFileFD()[i]->getFD());
		delete (this->_request.getUploadFileFD()[i]);
	}
	this->_request.getUploadFileFD().clear();
	for (size_t i = 0; i < this->_response._fd_files.size(); i++) {
		poll_event.deletePollFD(this->_response._fd_files[i]->getFD());
		delete (this->_response._fd_files[i]);
	}
	this->_response._fd_files.clear();
}

const std::string&	ClientFD::getErrorCode( void ) {
	return (this->_error_code);
}

const std::string&	ClientFD::getFDType( void ) const {
	return (this->_type);
}

void	ClientFD::setFD( int fd ) {
	this->_fd = fd;
}

int&	ClientFD::getFD( void ) {
	return (this->_fd);
}

short	ClientFD::getEvents( void ) const {
	return (this->_event);
}

int	ClientFD::getServerFd( void ) {
	return (this->_server_fd);
}

bool	ClientFD::isTimeout( time_t timeout ) {
	if (std::time(NULL) - this->_last_time_activity < timeout)
		return (false);
	return (true);
}

std::string&	ClientFD::getBuffer( void ) {
	return (this->_buffer);
}

void	ClientFD::setCloseYes( void ) {
	this->_should_close = true;
}

bool	ClientFD::shouldClose( void ) {
	return (this->_should_close);
}

Response&	ClientFD::getResponse( void ) {
	return (this->_response);
}

void	ClientFD::setResponse( const Response & rsp ) {
	this->_response = rsp;
}

Request&	ClientFD::getRequest( void ) {
	return (this->_request);
}

void	ClientFD::setRequest( const Request & rqst ) {
	this->_request = rqst;
}

std::string	ClientFD::checkRequestError( IPollFD& poll_event) {
	if (this->_error_code != OK)
		return (this->_error_code);
	if (this->_content_length > poll_event.getServer(this->_server_fd)->getMaxBodySize()) { 
		this->_error_code = PAYLOAD_TOO_LARGE;
		return (PAYLOAD_TOO_LARGE);
	}
	if (isTimeout(REQUEST_TIMEOUT)) {
		this->_error_code = REQUEST_TIMEOUT_ERROR;
		return (REQUEST_TIMEOUT_ERROR);
	}
	return (OK);
}

void	ClientFD::handleEvent( IPollFD& poll_event ) {

	try {
		if (this->_action == NONE) {
			this->updateLastTimeActivity();
			this->_action = GETTING_REQUEST;
		}
		if (this->_action == GETTING_REQUEST) {
			short	revent = poll_event.getFdRevent(this->getFD());

			if (this->checkRequestError(poll_event) != OK) {
				poll_event.setFdEvent(this->getFD(), POLLOUT);
				this->_action = SETTING_REQUEST;
				return ;
			}
			if ((revent & POLLIN)) {
				if (!getRequestString(this))
					return ;
				poll_event.setFdEvent(this->getFD(), POLLOUT);
				this->_action = SETTING_REQUEST;
			}
			if ((revent & POLLHUP) || (revent & POLLERR) || (revent & POLLNVAL))
				throw std::logic_error("Error:Invalid client fd\n");
		}
		if (this->_action == SETTING_REQUEST) {
			this->setRequest(Request(this->getServerFd(), poll_event, this->getBuffer()));
			this->_request.setAll(this->_error_code);
			this->_request.showTrueInfoRequest();
			this->setResponse(Response(this->getServerFd(), this->getFD(), poll_event, this->_request));

			const std::map<std::string, std::string>& cookies = this->_request.getCookies();
			std::string session_cookie_name = "WEBSERVSESSION";
			std::string session_id = "";
			std::map<std::string, std::string>::const_iterator itc = cookies.find(session_cookie_name);
			if (itc != cookies.end() && !itc->second.empty() && SessionManager::hasSession(itc->second)) {
				session_id = itc->second;
				SessionManager::touchSession(session_id);
			} else {
				session_id = SessionManager::createSession();
				std::string set_cookie = session_cookie_name + "=" + session_id + "; Path=/; HttpOnly";
				this->_response.addSetCookie( set_cookie );
			}

			this->_request.setSessionId(session_id);
			this->_buffer.clear();
			this->_action = SETTING_FILEFD;
		} 
		if (this->_action == SETTING_FILEFD) {
			this->_response.createFilesResponse();
			for (size_t j = 0; j < this->_request.getUploadFileFD().size() ; j++) {
				poll_event.addPollFd(this->_request.getUploadFileFD()[j]->getFD(), \
					this->_request.getUploadFileFD()[j]->getEvents());
			}
			for (size_t j = 0; j < this->_response._fd_files.size() ; j++) {
				poll_event.addPollFd(this->_response._fd_files[j]->getFD(), \
					this->_response._fd_files[j]->getEvents());
			}
			if (this->_request.isUploadType() && this->_request.getCodeError() == "201") {
				this->_response.setResponseType(RESPONSE_UPLOAD_WITHOUT_CGI);
			}
			this->updateLastTimeActivity();
			this->_action = SENDING_RESPONSE;
		}
		if (this->_action == SENDING_RESPONSE) {
			if (!this->_response.sendResponse(this->getFD(), \
				this->_response.getResponseType(), poll_event)) {
				if (this->isTimeout(RESPONSE_TIMEOUT))
					throw std::logic_error(GATEWAY_TIMEOUT_ERROR);
			}
			else
				this->setCloseYes();
		}
	}
	catch(const std::exception& e) {
		this->deleteFilesResponse(poll_event);
		if (static_cast<std::string>(e.what()) == GATEWAY_TIMEOUT_ERROR) {
			this->_response.killPid();
			this->_response.buildErrorPage("Gateway Timeout Error", GATEWAY_TIMEOUT_ERROR, "Gateway Timeout Error");
		}
		else if (static_cast<std::string>(e.what()) == INTERNAL_SERVER_ERROR)
			this->_response.buildErrorPage("Internal Server Error", INTERNAL_SERVER_ERROR, "Internal Server Error");
		else {
			WebServer::display(STDERR_FILENO, static_cast<std::string>(e.what()), RED);
			this->setCloseYes();
			return ;
		}
		for (size_t j = 0; j < this->_response._fd_files.size() ; j++)
			poll_event.addPollFd(this->_response._fd_files[j]->getFD(), this->_response._fd_files[j]->getEvents());
		this->updateLastTimeActivity();
		this->_action = SENDING_RESPONSE;
	}
}

bool	ClientFD::isCompleteRequest( std::string const& buffer ) {
	
	size_t pos = buffer.find("\r\n\r\n");
	if (pos == std::string::npos)
		return (false);
	
	if (!this->_content_length) {
		std::string 		header = buffer.substr(0, pos + 4);
		std::istringstream	iss(header);
		std::string 		line;
		
		while (std::getline(iss, line)) {
			if (line.find("Content-Length:") != std::string::npos) {
				size_t colon = line.find(":");
				if (colon != std::string::npos) {
					this->_content_length = std::atoi(line.substr(colon + 1).c_str());
				}
			}
		}
	}

	size_t total_expected = pos + 4 + this->_content_length;
	return (buffer.size() >= total_expected);
}

bool	ClientFD::getRequestString( ClientFD*	client ) {
	char		buf[BUFSIZ];

	std::memset(buf, 0, BUFSIZ);
	int	 len = recv(client->getFD(), buf, BUFSIZ - 1, 0);
	if (len <= 0) {
		throw std::logic_error("Error: closed client fd\n");
	}
	client->getBuffer().append(buf, len);
	return (this->isCompleteRequest(this->getBuffer()));
}

bool	ClientFD::tooLargeContentLength( std::string const& cl, IPollFD& poll_event ) {
	size_t	len = std::atoi(cl.c_str());
	size_t	max = poll_event.getServer(this->_server_fd)->getMaxBodySize();

	if (len > max)
		return (true);
	return (false);
}
