#include "FileFD.hpp"
#include "WebServer.hpp"

/**
 * ********************** Coplien *******************
 */

FileFD::FileFD( void ) {
	this->_type = "file";
	this->_tmp = "";
	this->_fd = 0;
	this->_event = POLLIN | POLLOUT | POLLERR | POLLHUP | POLLNVAL;
}

FileFD::FileFD( int& fd ) {
	this->_type = "file";
	this->_fd = fd;
	this->_event = POLLIN | POLLOUT | POLLERR | POLLHUP | POLLNVAL;
	this->_should_close = false;
	this->_tmp = "";
}

FileFD::FileFD( const FileFD& cpy ) {
	(*this) = cpy;
}

FileFD::FileFD( int& fd, short event ) {
	this->_type = "file";
	this->_fd = fd;
	this->_event = event | POLLERR | POLLHUP | POLLNVAL;
	this->_should_close = false;
	this->_tmp = "";
	this->_content.clear();
}

FileFD::FileFD( int& fd, std::string const& str, short event ) {
	this->_type = "file";
	this->_fd = fd;
	this->_event = event | POLLERR | POLLHUP | POLLNVAL;
	this->_should_close = false;
	this->_tmp = "";
	this->_content = str;
}

FileFD::~FileFD( void ) {
		WebServer::closeFD(this->_fd);
}

FileFD&	FileFD::operator=( const FileFD& cpy ) {
	this->_type = cpy._type;
	this->_fd = cpy._fd;
	this->_event = cpy._event;
	this->_tmp = cpy._tmp;
	this->_content = cpy._content;
	return (*this);
}






/**
 * ********************* PUBLIC METHODS ***********************
 */

const std::string&	FileFD::getFDType( void ) const {
    return (this->_type);
}

void	FileFD::setFD( int fd ) {
	this->_fd = fd;
}

int&	FileFD::getFD( void ) {
	return (this->_fd);
}

short	FileFD::getEvents( void ) const {
	return (this->_event);
}

bool	FileFD::writeContent( void ) {
	if (this->_content.empty())
		return (true);
	this->_tmp = this->_content.substr(0, BUFSIZZ);
	if (write(this->_fd, this->_tmp.c_str(), this->_tmp.size()) == -1)
		throw std::logic_error(INTERNAL_SERVER_ERROR);

	this->_content.erase(0, this->_tmp.size());
	if (this->_content.empty())
		return (true);
	return (false);
}

bool	FileFD::shouldClose( void ) {
	return (this->_should_close);
}

void	FileFD::setCloseYes( void ) {
	this->_should_close = true;
}

std::string&	FileFD::getContent( void ) {
	return (this->_content);
}
