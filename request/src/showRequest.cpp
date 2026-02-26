/* 
	showRequest.cpp
 */

 #include "Request.hpp"

 void	Request::showTrueInfoRequest( void ) {
	std::string	str = this->getMethod();
	WebServer::display( STDOUT_FILENO, str, BG_BLUE );
	str = "  " + this->getPath() + "  ";
	WebServer::display( STDOUT_FILENO, str, BRIGHT_GREEN );
	str = this->getVersion() + "\n";
	WebServer::display( STDOUT_FILENO, str, BRIGHT_MAGENTA );
 }

void	Request::showHeaders( void ) {
	this->showPostHeader();
	for (std::map<std::string, std::string>::const_iterator it = this->_headers.begin(); it != this->_headers.end(); ++it) {
		std::string str = it->first;
		WebServer::display( STDOUT_FILENO, str, BG_BLUE );
		str = it->second + "\n";
		WebServer::display( STDOUT_FILENO, str, BRIGHT_GREEN );
	}
}

void	Request::showKeyValue( void ) {
	this->showPostHeader();
	for (std::map<std::string, std::string>::const_iterator it = this->_key_value.begin(); it != this->_key_value.end(); ++it) {
		std::string str = "KEY::";
		WebServer::display( STDOUT_FILENO, str, BG_BLUE );
		str = it->first;
		WebServer::display( STDOUT_FILENO, str, BRIGHT_GREEN );
		str = "VALUE::";
		WebServer::display( STDOUT_FILENO, str, BG_BLUE );
		str = it->second + CRLF;
		WebServer::display( STDOUT_FILENO, str, BRIGHT_GREEN );
	}
}

void	Request::showAll( void ) {
	this->showRequest();
	this->showTrueInfoRequest();
	this->showHeaders();
	this->showKeyValue();
	this->showBody();
}

void	Request::showRequest( void ) {
	this->showPostHeader();
	WebServer::display( STDOUT_FILENO, *(this->_request), BG_BLUE );
}

void	Request::showBody( void ) {
	this->showPostHeader();
	std::string str = this->getBody() + CRLF;
	WebServer::display( STDOUT_FILENO, str, BG_BLUE );
}

void	Request::showPostHeader( void ) {
	std::string str = "\r\nOUTPUT POST\n";
	WebServer::display( STDOUT_FILENO, str, BG_RED );
}

void	Request::showCTPCTPCL( void ) {
	this->showPostHeader();
	std::string str = "Content-Type";
	WebServer::display( STDOUT_FILENO, str, BG_BLUE );
	str = this->getContentType() + CRLF;
	WebServer::display( STDOUT_FILENO, str, BRIGHT_GREEN );
	str = "Post Content-Type";
	WebServer::display( STDOUT_FILENO, str, BG_BLUE );
	str = this->getPostContentType() + CRLF;
	WebServer::display( STDOUT_FILENO, str, BRIGHT_GREEN );
	str = "Post Content-Length";
	WebServer::display( STDOUT_FILENO, str, BG_BLUE );
	str = this->getPostContentLength() + CRLF;
	WebServer::display( STDOUT_FILENO, str, BRIGHT_GREEN );
}

void	Request::showCookies( void ) {
	this->showPostHeader();
	for (std::map<std::string, std::string>::const_iterator it = this->_cookies.begin(); it != this->_cookies.end(); ++it) {
		std::string str = "COOKIE KEY::";
		WebServer::display( STDOUT_FILENO, str, BG_BLUE );
		str = it->first;
		WebServer::display( STDOUT_FILENO, str, BRIGHT_GREEN );
		str = "COOKIE VALUE::";
		WebServer::display( STDOUT_FILENO, str, BG_BLUE );
		str = it->second + CRLF;
		WebServer::display( STDOUT_FILENO, str, BRIGHT_GREEN );
	}
}
