#include "PollFD.hpp"


/**
 * 
 * ********************* Coplien ********************
 */

PollFD::PollFD( void ) {
	
}

PollFD::PollFD( const PollFD& cpy ) {
	(*this) = cpy;
}

PollFD::~PollFD( void ) {}

PollFD::PollFD( std::vector<ServerFD*>& afds ) {
	this->_all_servers = afds;
}

PollFD&	PollFD::operator=( const PollFD& cpy ) {
	this->_poll_fds = cpy._poll_fds;
	this->_all_servers = cpy._all_servers;
	return (*this);
}







/**
 * 
 * ********************* PUBLIC METHODS ***********************
 * f
 */

const std::string&	PollFD::getConfigFile( void ) {
	return (this->_config_filename);
}

void	PollFD::setConfigFile( const std::string& filename ) {
	this->_config_filename = filename;
}

void	PollFD::addPollFd( int fd, short event ) {
	struct pollfd	pfd = {fd, event, 0 };
	this->_poll_fds.push_back(pfd);
}

short	PollFD::getFdRevent( int fd ) {
	for (std::vector<struct pollfd>::iterator it = this->_poll_fds.begin(); 
		it != this->_poll_fds.end(); it++) {
		if ((*it).fd != fd)
			continue;
		return ((*it).revents);
	}
	return (0);
}

void	PollFD::update( void ) {
	this->_poll_fds.clear();

	struct pollfd	cout = {STDOUT_FILENO, POLLOUT, 0};
	struct pollfd	cerr = {STDERR_FILENO, POLLOUT, 0};
	
	this->_poll_fds.push_back(cout);
	this->_poll_fds.push_back(cerr);
	for (size_t i = 0; i < this->_all_servers.size(); i++) {
		struct pollfd	pfd;
		pfd.fd = this->_all_servers[i]->getFD();
		pfd.events = POLLIN | POLLERR | POLLHUP | POLLNVAL;
		pfd.revents = 0;
		this->_poll_fds.push_back(pfd);
	}
}

struct pollfd&	PollFD::getPollFD( int i )  {
	return this->_poll_fds[i];
}

ServerFD*	PollFD::getServer( int fd ) {
	for (size_t i = 0; i < this->_all_servers.size(); i++) {
		if (this->_all_servers[i]->getFD() != fd)
			continue;
		return (this->_all_servers[i]);	
	}
	return (NULL);
}

std::vector<ServerFD*>&	PollFD::getAllServer( void ) {
	return (this->_all_servers);
}

std::vector<struct pollfd>&	PollFD::getAllPollFD( void ) {
	return (this->_poll_fds);
}

void	PollFD::setFdEvent( int fd, short event ) {
	for (size_t i = 0; i < this->_poll_fds.size(); i++) {
		if (this->_poll_fds[i].fd != fd)
			continue;
		this->_poll_fds[i].events |= event;
		return ;
	}
}


void	PollFD::deletePollFD( int fd ) {
	for (size_t i = 0; i < this->_poll_fds.size(); i++) {
		if (this->_poll_fds[i].fd != fd)
			continue;
		this->_poll_fds.erase(this->_poll_fds.begin() + i);
		break;
	}
}
