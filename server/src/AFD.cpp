#include "AFD.hpp"


/**
 * ********************** Coplien *******************
 */

AFD::~AFD( void ) {}


/**
 * ********************* PUBLIC METHODS ***********************
 */

void	AFD::display( void ) {}

void	AFD::writeFD( std::string const& str ) {
	(void)str;
}

std::string const&	AFD::getServerName( void ) const {
	return (this->_type);
}

std::string const&	AFD::getPort( void ) const {
	return (this->_type);
}

std::string const &	AFD::getFDType( void ) const {
	return (this->_type);
}

std::string const&	AFD::getErrorIndex( int error ) const {
	(void)error;
	return (this->_type);
}

std::string const&	AFD::getRoot( std::string const& uri ) {
	(void)uri;
	return (this->_type);
}

std::string const&	AFD::getIndex( std::string const& uri ) {
	(void)uri;
	return (this->_type);
}

bool	AFD::isAllowed( std::string const& uri , std::string const& method ) {
	(void)uri;
	(void)method;
	return (false);
}

bool	AFD::autoIndexON( std::string const& uri ) {
	(void)uri;
	return (false);
}

bool	AFD::cookiesSupportON( std::string const& uri ) {
	(void)uri;
	return (false);
}

std::string const&	AFD::getRedirectPath( std::string const& uri ) {
	(void)uri;
	return (this->_type);
}

std::string const&	AFD::getRedirectCode( std::string const& uri ) {
	(void)uri;
	return (this->_type);
}

void	AFD::setFD( int fd ) {
	this->_fd = fd;
}

int&	AFD::getFD( void ) {
	return (this->_fd);
}

size_t	AFD::getRequestSize( void ) const {
	return (0);
}

size_t	AFD::getMaxBodySize( void ) const {
	return (0);
}

short	AFD::getEvents( void ) const {
	return (this->_event);
}

void	AFD::handleEvent( IPollFD& poll_event ) {
	(void)poll_event;
}

bool	AFD::shouldClose( void ) {
	return (false);
}

void	AFD::setCloseYes( void ) {

}

std::string const&	AFD::getLocationUri( std::string const& uri ) {
	(void)uri;
	return (this->_type);
}








/**
 * ********************* PRIVATE METHODS ***********************
 */

//	VOID