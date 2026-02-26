#include "LocationConfig.hpp"

/**
 * ****************** Coplien ******************
 */

LocationConfig::LocationConfig( void ) {}

LocationConfig::LocationConfig( std::string const& uri ) {
	this->clear();
	this->_uri = uri;
}

LocationConfig::LocationConfig( const LocationConfig& cpy ) {
	(*this) = cpy;
}

LocationConfig::~LocationConfig( void ) {
	
}

LocationConfig&	LocationConfig::operator=( const LocationConfig& cpy ) {
	this->clear();
	this->_tmp = "";
	this->_autoindex = cpy._autoindex;
	this->_cookies_support = cpy._cookies_support;
	this->_full_path = cpy._full_path;
	this->_index = cpy._index;
	this->_methodStat = cpy._methodStat;
	this->_required_key = cpy._required_key;
	this->_root = cpy._root;
	this->_uri = cpy._uri;
	this->_redirect_path = cpy._redirect_path;
	this->_redirect_code = cpy._redirect_code;
	this->_upload_dir = cpy._upload_dir;
	this->_all_cgi = cpy._all_cgi;
	return (*this);
}










/**
 * ******************** PUBLIC Methods ********************
 */

void	LocationConfig::clear() {
	this->_autoindex = false;
	this->_cookies_support = false;
	this->_redirect_path.clear();
	this->_redirect_code.clear();
	this->_full_path.clear();
	this->_index.clear();
	this->_methodStat.clear();
	this->_root.clear();
	this->_upload_dir.clear();
	this->_uri.clear();
	this->_tmp.clear();
	this->_methodStat.clear();
	this->_required_key[COOKIES_SUPPORT] = false;
	this->_required_key[HTTP_METHODS] = false;
	this->_required_key[UPLOAD_DIR] = false;
	this->_required_key[AUTOINDEX] = false;
	this->_required_key[REDIRECT] = false;
	this->_required_key[INDEX] = false;
	this->_required_key[ROOT] = false;
	this->_required_key[CGI] = false;
}

void	LocationConfig::setLocationBloc( \
	std::vector< std::vector<std::string> >::iterator&   it_loc ) {

	//	get one server block from config

	while ((*it_loc).size() && (*it_loc).at(0) != SERVER && (*it_loc).at(0) != LOCATION) {
		if ((*it_loc).size() < 2 || !this->isLocationWordKey((*it_loc).at(0))) {
			std::string error = "Content_error: on config file (undefined \'" + \
				(*it_loc).at(0) + "\' key)" ;
			throw std::invalid_argument(error);
		}
	
		//	set attributs
		if ((*it_loc).at(0) == REDIRECT)
			this->setRedirection((*it_loc));
		if ((*it_loc).at(0) == AUTOINDEX)
			this->setAutoIndex((*it_loc));
		if ((*it_loc).at(0) == COOKIES_SUPPORT)
			this->setCookiesSupport((*it_loc));
		else if ((*it_loc).at(0) == HTTP_METHODS) 
			this->setMethods((*it_loc));
		else if ((*it_loc).at(0) == ROOT) 
			this->setRoot((*it_loc));
		else if ((*it_loc).at(0) == INDEX) 
			this->setIndex((*it_loc));
		else if ((*it_loc).at(0) == CGI) 
			this->setCgi((*it_loc));
		else if ((*it_loc).at(0) == UPLOAD_DIR) 
			this->setUploadDir((*it_loc));
		if ((*it_loc).at((*it_loc).size() - 1) == "}") {
			it_loc++;
			break;
		}
		it_loc++;
	}
	this->_full_path = this->_root + this->_uri;
	it_loc--;
	if (!this->_required_key[ROOT] && !this->_required_key[REDIRECT])
		throw std::invalid_argument("Content_error: missing root (location bloc)"); 
}

const std::string&	LocationConfig::getCgiPass( std::string const& extension ) {
	try {
		return (this->_all_cgi[extension]);
	}
	catch(const std::exception& e) {
		return (this->_tmp);
	}	
}

const std::string&	LocationConfig::getIndex( void ) {
	return (this->_index);
}

const std::string&	LocationConfig::getRoot( void ) {
	return (this->_root);
}

const std::string&	LocationConfig::getUri( void ) {
	return (this->_uri);
}

const std::string&	LocationConfig::getUploadDir( void ) {
	return (this->_upload_dir);
}

bool	LocationConfig::isAllowedMethod( const std::string& method ) {
	try {
		return (this->_methodStat.at(method));
	}
	catch(const std::exception& e) {
		return (false);
	}
	return (false);
}

bool	LocationConfig::getAutoIndex( void ) {
	return (this->_autoindex);
}

bool	LocationConfig::getCookiesSupport( void ) {
	return (this->_cookies_support);
}

const std::string&	LocationConfig::getRedirectPath( void ) {
	return this->_redirect_path;
}

std::string const&	LocationConfig::getRedirectCode( void ) {
	return this->_redirect_code;
}




         



/**
 * ******************** PRIVATE Methods ********************
 */


bool	LocationConfig::isLocationWordKey( const std::string& str ) {
	return ( str == HTTP_METHODS || str == ROOT || str == INDEX || str == COOKIES_SUPPORT || \
		str == CGI || str == AUTOINDEX || str == REDIRECT || str == UPLOAD_DIR);
}



void	LocationConfig::setMethods( std::vector<std::string>& content ) {

	//	set Http methods

	std::vector<std::string>::iterator	it;
	it = content.begin();
	it++;
	for (; it != content.end() && (*it) != ";"; it++) {
		this->_methodStat[*it] = true;
	}
	this->_required_key[HTTP_METHODS] = true;
}


void	LocationConfig::setRedirection( std::vector<std::string>& content ) {

	//	set redirection
	std::stringstream	ss;
	int					code;

	if (this->_required_key[REDIRECT] || WebServer::directiveLen(content) != 3)
		throw std::invalid_argument("Content_error: on config file (return)");
	ss << content.at(1);
	ss >> code;
	if (ss.fail() || !ss.eof() || ((code < 301 || code > 303) && \
		(code < 307 || code > 308)))
		throw std::invalid_argument("Content_error: on config file (return) -> not a valid redirection code");
	this->_redirect_code = content.at(1);
	this->_redirect_path = content.at(2);
	this->_required_key[REDIRECT] = true;
}

void	LocationConfig::setRoot( std::vector<std::string>& content ) {

	//	set root

	if (this->_required_key[ROOT] || WebServer::directiveLen(content) != 2)
		throw std::invalid_argument("Content_error: on config file (root)");
	this->_root = content.at(1);
	WebServer::sliceSlash(this->_root);
	this->_required_key[ROOT] = true;
}

void	LocationConfig::setIndex( std::vector<std::string>& content ) {

	//	setIndex

	if (this->_required_key[INDEX] || WebServer::directiveLen(content) != 2)
		throw std::invalid_argument("Content_error: on config file (index)");
	this->_index = content.at(1);
	this->_required_key[INDEX] = true;
}

void	LocationConfig::setCgi( std::vector<std::string>& content ) {

	//	set CGI 

	if (WebServer::directiveLen(content) != 3)
		throw std::invalid_argument("Content_error: on config file (cgi)");
	this->_all_cgi.insert(std::make_pair(content.at(1), content.at(2)));
	this->_required_key[CGI] = true;
}

void	LocationConfig::setAutoIndex( std::vector<std::string>& content ) {

	//	set AUTOINDEX

	if (this->_required_key[AUTOINDEX] || WebServer::directiveLen(content) != 2 || \
		(content.at(1) != "off" && content.at(1) != "on"))
		throw std::invalid_argument("Content_error: on config file (auto_index)");
	this->_autoindex = (content.at(1) == "on") ? true : false;
	this->_required_key[AUTOINDEX] = true;
}

void	LocationConfig::setCookiesSupport( std::vector<std::string>& content ) {

	//	set COOKIES_SUPPORT

	if (this->_required_key[COOKIES_SUPPORT] || WebServer::directiveLen(content) != 2 || \
		(content.at(1) != "off" && content.at(1) != "on"))
		throw std::invalid_argument("Content_error: on config file (auto_index)");
	this->_cookies_support = (content.at(1) == "on") ? true : false;
	this->_required_key[COOKIES_SUPPORT] = true;
}

void	LocationConfig::setUploadDir( std::vector<std::string>& content ) {

	//	set UPLOAD_DIR

	if (this->_required_key[UPLOAD_DIR] || WebServer::directiveLen(content) != 2)
		throw std::invalid_argument("Content_error: on config file (upload_dir)");
	this->_upload_dir = content.at(1);
	this->_required_key[UPLOAD_DIR] = true;
}
