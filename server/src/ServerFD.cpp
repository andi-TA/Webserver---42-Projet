#include "ServerFD.hpp"
#include "ClientFD.hpp"


/**
 * ****************** Coplien ******************
 */

ServerFD::ServerFD( void ) {

	//	set up all of the attributs stats to false = don't exist yet
	this->_required_key.insert(std::pair<std::string, bool>(ERROR_PAGE, false));
	this->_required_key.insert(std::pair<std::string, bool>(LOCATION, false));
	this->_required_key.insert(std::pair<std::string, bool>(PORT, false));
	this->_required_key.insert(std::pair<std::string, bool>(REDIRECT, false));
	this->_required_key.insert(std::pair<std::string, bool>(MAX_BODY_SIZE, false));
    this->_type = "server";
    this->_tmp = "";
	this->_fd = 0;
	this->_event = POLLIN | POLLERR | POLLHUP | POLLNVAL;
	this->_multiple_port.clear();
}

ServerFD::ServerFD( const ServerFD& cpy ) {
	
	//	copy constructor
	(*this) = cpy;
}

ServerFD::~ServerFD( void ) {
	for (std::map<int, ClientFD*>::iterator it = this->getAllClient().begin() ;
		it != this->getAllClient().end();) {
		delete ((*it).second);
		this->getAllClient().erase(it);
		it = this->getAllClient().begin();
	}
	this->_clients.clear();
	WebServer::closeFD(this->_fd);
}

ServerFD&	ServerFD::operator=( const ServerFD& cpy ) {
	
	//	copy assignement
	this->clear();
	this->_tmp = cpy._tmp;
	this->_max_body_size = cpy._max_body_size;
	this->_request_body_size = cpy._request_body_size;
	this->_error_page = cpy._error_page;
	this->_locations = cpy._locations;
	this->_port = cpy._port;
	this->_required_key = cpy._required_key;
	this->_host = cpy._host;
    this->_type = cpy._type;
	this->_fd = cpy._fd;
	this->_event = cpy._event;
	this->_redirect = cpy._redirect;
	this->_right_location = cpy._right_location;
	this->_multiple_port = cpy._multiple_port;
	return (*this);
}









/**
 * ******************** PUBLIC Methods ********************
 */

std::string const&	ServerFD::getFDType( void ) const {
	return (this->_type);
}

void	ServerFD::insertLocationBlock( std::vector< \
	std::vector<std::string> >::iterator&   it_serv ) {

	LocationConfig	location_tmp = LocationConfig((*it_serv++).at(1));

	location_tmp.setLocationBloc(it_serv);
	this->_locations.push_back(location_tmp);
	this->_required_key[LOCATION] = true;
}

void	ServerFD::setServerBloc( \
	std::vector< std::vector<std::string> >::iterator&   it_serv ) {

	//	get one server block from config

	this->clear();
	while ((*it_serv).size() && (*it_serv).at(0) != SERVER) {
		if ((*it_serv).size() < 2 || !this->isServerWordKey((*it_serv).at(0))) {
			std::string error = "Content_error: on config file (undefined \'" + \
				(*it_serv).at(0) + "\' key)" ;
			throw std::invalid_argument(error);
		}

		//	set attributs
		if ((*it_serv).at(0) == PORT)
			this->setMultiPort(*it_serv);
		else if ((*it_serv).at(0) == MAX_BODY_SIZE)
			this->setMaxBodySize( *it_serv );
		else if ((*it_serv).at(0) == ERROR_PAGE)
			this->setErrorPage(*it_serv);
		else if ((*it_serv).at(0) == LOCATION) {
			if (WebServer::directiveLen((*it_serv)) != 2)
				throw std::invalid_argument("Content_error: on config file (location URI)");
			this->insertLocationBlock(it_serv);
		}
		it_serv++;
	}
	if (!this->_required_key[PORT])
		throw std::invalid_argument("Content_error: missing host port (server)");
	if (!this->_required_key[MAX_BODY_SIZE])
		this->_max_body_size = std::string::npos;
}

const std::string&	ServerFD::getPort( void ) {
	return (this->_port);
}

const std::string&	ServerFD::getHost( void ) {
	return (this->_host);
}

std::vector<std::string>&	ServerFD::getMultiPort( void ) {
	return (this->_multiple_port);
}

void	ServerFD::setFD( int fd ) {
	this->_fd = fd;
}

void	ServerFD::setHost( std::string const& host ) {
	this->_host = host;
}

void	ServerFD::setPort( std::string const& port ) {
	this->_port = port;
}

int&	ServerFD::getFD( void ) {
	return (this->_fd);
}

short	ServerFD::getEvents( void ) const {
	return (this->_event);
}

void	ServerFD::handleEvent( IPollFD& poll_event ) {
	
	struct sockaddr_in	client;
	socklen_t			len = sizeof(client);
	int		clientfd = accept(this->getFD(), (struct sockaddr*)&client, &len);
	if (clientfd == -1) {
		WebServer::display(STDERR_FILENO, "Error : Client Acception Error", RED);
		return ;
	}
	
	try {
		ClientFD*	cl_tmp = new ClientFD(clientfd, this->getFD());
	
		this->addClient(cl_tmp);
		poll_event.addPollFd(clientfd, POLLIN);
	}
	catch(const std::exception& e) {
		WebServer::display(STDERR_FILENO, "Error : Client allocation", RED);
	}
}

void	ServerFD::addClient( ClientFD* new_client ) {
	this->_clients.insert(std::make_pair(new_client->getFD(), new_client));
}

ClientFD*	ServerFD::getClientByFD( int fd ) {
	try {
		return (this->_clients.at(fd));
	}
	catch(const std::exception& e) {
		return (NULL);
	}
	return (NULL);
}

std::map<int, ClientFD*>&	ServerFD::getAllClient( void ) {
	return (this->_clients);
}






// ---------- GETTERS ------------

std::string const&	ServerFD::getHost( void ) const {

	//	GET Server name
	return (this->_host);
}

std::string const&	ServerFD::getPort( void ) const {

	//	GET Port number
	return (this->_port);
}

std::vector<LocationConfig>	ServerFD::getLocations( void ) const {

	//	GET location tab
	return (this->_locations);
}

std::map<std::string, std::string> const&	ServerFD::getErrorIndex( void ) const {

	//	GET error 
	return (this->_error_page);
}

std::string const	ServerFD::getUploadDir( std::string const& uri ) {

	//	get upload_dir from uri
	std::string	right_uri = this->searchRightUri(uri);

	for (std::vector<LocationConfig>::iterator it = this->_locations.begin(); \
		it != this->_locations.end(); it++) {
		if (it->getUri() == right_uri) {
			if (it->getUploadDir().empty())
				return (this->_tmp);
			return (it->getRoot() + "/" + it->getUploadDir());
		}
	}
	return (this->_tmp);
}

std::string const&	ServerFD::getSimpleUploadDir( std::string const& uri ) {

	//	get upload_dir from uri
	std::string	right_uri = this->searchRightUri(uri);

	for (std::vector<LocationConfig>::iterator it = this->_locations.begin(); \
		it != this->_locations.end(); it++) {
		if (it->getUri() == right_uri) {
			if (it->getUploadDir().empty())
				return (this->_tmp);
			return (it->getUploadDir());
		}
	}
	return (this->_tmp);
}

std::string const&	ServerFD::getRoot( std::string const& uri ) {
	
	//	get root path from uri
	std::string	right_uri = this->searchRightUri(uri);

	for (std::vector<LocationConfig>::iterator it = this->_locations.begin(); \
		it != this->_locations.end(); it++) {
		if (it->getUri() == right_uri)
			return (it->getRoot());
	}
	return (this->_tmp);
}

std::string const&	ServerFD::getIndex( std::string const& uri ) {
	
	//	get index file from uri
	std::string	right_uri = this->searchRightUri(uri);

	for (std::vector<LocationConfig>::iterator it = this->_locations.begin(); \
		it != this->_locations.end(); it++) {
		if (it->getUri() == right_uri)
			return (it->getIndex());
	}
	return (this->_tmp);
}

std::string const&	ServerFD::getCgiPass( std::string const& uri, std::string const& extension ) {
	std::string	right_uri = this->searchRightUri(uri);

	for (std::vector<LocationConfig>::iterator it = this->_locations.begin(); \
		it != this->_locations.end(); it++) {
		if (it->getUri() == right_uri)
			return (it->getCgiPass(extension));
	}
	return (this->_tmp);
}

bool	ServerFD::isAllowed( std::string const& uri , std::string const& method ) {
	
	//	get method stat from uri
	std::string	right_uri = this->searchRightUri(uri);

	for (std::vector<LocationConfig>::iterator it = this->_locations.begin(); \
		it != this->_locations.end(); it++) {
		if (it->getUri() == right_uri)
			return (it->isAllowedMethod(method));
	}
	return (false);
}

std::string const&	ServerFD::getRedirectPath( std::string const& uri ){
	
	//	get redirection from uri
	std::string	right_uri = this->searchRightUri(uri);

	for (std::vector<LocationConfig>::iterator it = this->_locations.begin(); \
		it != this->_locations.end(); it++) {
		if (it->getUri() == right_uri)
			return (it->getRedirectPath());
	}
	return (this->_tmp);
}

std::string const&	ServerFD::getRedirectCode( std::string const& uri ){
	
	//	get redirection from uri
	std::string	right_uri = this->searchRightUri(uri);

	for (std::vector<LocationConfig>::iterator it = this->_locations.begin(); \
		it != this->_locations.end(); it++) {
		if (it->getUri() == right_uri)
			return (it->getRedirectCode());
	}
	return (this->_tmp);
}

bool	ServerFD::autoIndexON( std::string const& uri ) {

	//	get autoIndex stat from uri
	std::string	right_uri = this->searchRightUri(uri);

	for (std::vector<LocationConfig>::iterator it = this->_locations.begin(); \
		it != this->_locations.end(); it++) {
		if (it->getUri() == right_uri)
			return (it->getAutoIndex());
	}
	return (false);
}

bool	ServerFD::cookiesSupportON( std::string const& uri ) {

	//	get cookies_support stat from uri
	std::string	right_uri = this->searchRightUri(uri);

	for (std::vector<LocationConfig>::iterator it = this->_locations.begin(); \
		it != this->_locations.end(); it++) {
		if (it->getUri() == right_uri)
			return (it->getCookiesSupport());
	}
	return (false);
}

size_t	ServerFD::getRequestSize( void ) const {
	return (this->_request_body_size);
}

std::string const&	ServerFD::getLocationUri( std::string const& uri ) {
	this->_right_location = this->searchRightUri(uri);
	return (this->_right_location);
}

size_t	ServerFD::getMaxBodySize( void ) const {
	return ( this->_max_body_size );
}








/**
 * ******************** PRIVATE Methods ********************
 */

void	ServerFD::clear() {
	
	//	Reset Server contents

	this->_request_body_size = 0;
	this->_error_page.clear();
	this->_locations.clear();
	this->_port.clear();
	this->_host.clear();
	this->_multiple_port.clear();
	for (std::map<std::string, bool>::iterator it = this->_required_key.begin(); \
		it != this->_required_key.end(); it++)
		it->second = false;
}

bool	ServerFD::isServerWordKey( const std::string& str ) {
	return ( str == ERROR_PAGE || str == PORT || \
		str == LOCATION || str == MAX_BODY_SIZE );
}

void	ServerFD::setMultiPort( std::vector<std::string>& content ) {
	
	//	Set server port

	std::stringstream	ss;
	std::string			tmp;
	int					nbr;

	if (WebServer::directiveLen(content) != 3)
		throw std::invalid_argument("Content_error: on config file (Port)");
	ss << content.at(2);
	ss >> nbr;
	if (ss.fail())
		throw std::invalid_argument("Content_error: on config file (port) -> not a number");
	if (nbr < PORT_MIN || nbr > PORT_MAX)
		throw std::invalid_argument("Content_error: on config file (port) -> out of range");
	ss >> tmp;
	if (tmp.length())
		throw std::invalid_argument("Content_error: on config file (port) -> not a number");	
	this->_host = content.at(1);
	this->_port = content.at(2);
	this->_multiple_port.push_back(this->_host + ":" + this->_port);
	this->_required_key[PORT] = true;
}

void	ServerFD::setMaxBodySize( std::vector<std::string>& content ) {
	std::stringstream	ss;
	std::string			tmp;
	size_t				nbr;

	if (this->_required_key[MAX_BODY_SIZE] || WebServer::directiveLen(content) != 2)
		throw std::invalid_argument("Content_error: on config file (Max body size)");
	ss << content.at(1);
	ss >> nbr;
	if (ss.fail())
		throw std::invalid_argument("Content_error: on config file (Max body size) -> not a number");
	if (nbr <= 0)
		throw std::invalid_argument("Content_error: on config file (Max body size) -> negative number");	
	ss >> tmp;
	if (tmp.length())
		throw std::invalid_argument("Content_error: on config file (Max body size) -> not a number");
	this->_max_body_size = nbr;
	this->_required_key[MAX_BODY_SIZE] = true;
}

void	ServerFD::setErrorPage( std::vector<std::string>& content ) {

	//	Add error pages

	std::stringstream	ss;
	std::string			tmp;
	int					nbr;

	if (WebServer::directiveLen(content) != 3)
		throw std::invalid_argument("Content_error: on config file (Error_page)");
	ss << content.at(1);
	ss >> nbr;
	if (ss.fail())
		throw std::invalid_argument("Content_error: on config file (Error_page id) -> not a number");
	ss >> tmp;
	if (tmp.length())
		throw std::invalid_argument("Content_error: on config file (Error_page id) -> not a number");
	WebServer::sliceSlash(content.at(2));
	this->_error_page.insert(std::pair<std::string, std::string>(content.at(1), content.at(2)));
	this->_required_key[ERROR_PAGE] = true;
}

std::string const	ServerFD::searchRightUri( std::string const& uri ) {
	std::vector<LocationConfig>::iterator	it;
	std::vector<LocationConfig>::iterator	it1;
	std::string								right_uri;

	for (it = this->_locations.begin(); it != this->_locations.end(); it++) {
		if ((uri.find(it->getUri()) == 0 && 
			(uri.length() == it->getUri().length() || (uri.length() > it->getUri().length() && 
			uri.at(it->getUri().length()) == '/'))) && 
			it->getUri().length() > right_uri.length())			
			right_uri = it->getUri();
	}
	if (!right_uri.length())
		right_uri = "/";
	return (right_uri);
}
