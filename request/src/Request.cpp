/*
	Request.cpp
*/

#include "Request.hpp"
#include <algorithm>
#include <cctype>

std::map< std::string, std::string >	Request::_mime_container;

Request::Request() : IRequest() {
	this->_server_fd = NULL;
}

Request::Request( const Request & other ) : IRequest() {
	*this = other;
}

const Request & Request::operator = ( const Request & other ) {
	if ( this != &other ) {
		this->_upload_file_fd = other._upload_file_fd;
		this->_server_fd = other._server_fd;
		this->_request = other._request;
		this->_is_upload_type = other._is_upload_type;
		this->_path = other._path;
		this->_method = other._method;
		this->_key_value = other._key_value;
		this->_content_type = other._content_type;
		this->_post_content_length = other._post_content_length;
	}
	return ( *this );
}

Request::Request( int server_fd, IPollFD& allFD, const std::string& request ) : IRequest(), _request( &request ) {
	this->_server_fd = allFD.getServer(server_fd);
	this->_is_upload_type = false;
}

Request::~Request() {}

std::string const&	Request::getPath( void ) const {
	return ( this->_path );
}

std::string const&	Request::getMethod( void ) const {
	return ( this->_method );
}

std::string const&	Request::getContentType( void ) const {
	return ( this->_content_type );
}

std::string const&	Request::getPostContentLength( void ) const {
	return ( this->_post_content_length );
}

const std::map<std::string, std::string>&	Request::getKeyValueInRequest( void ) const {
	return ( this->_key_value );
}

void	Request::setAll( std::string const& status ) {
	if (status == REQUEST_TIMEOUT_ERROR) {
		this->setCodeAndMessageError("408", "Request Timeout");
		return ;
	}
	if (status == PAYLOAD_TOO_LARGE) {
		this->setCodeAndMessageError( "413", "Payload Too Large" );
		return ;
	}
	this->parseHeaders();
	this->setMimeContainer();
	this->setPath();
	this->setMethod();
	this->setVersion();
	this->setFullPath();
	this->setQueryString();
	this->setContentType();
	this->setPostContentLength();
	this->checkRequestError();
	if ( !this->isRequestError() ) {
		this->setKeyValueInRequest();
		int status = 0;
		this->setDelete( this->_full_path, status );
	}
}

bool	Request::isUploadType( void ) const {
	return (this->_is_upload_type);
}

std::vector< FileFD* >&	Request::getUploadFileFD( void ) {
	return (this->_upload_file_fd);
}

std::string	const &	Request::getQueryString( void ) const {
	return ( this->_query_string );
}

std::string const & Request::getContentTypeWithParam(const std::string& file) const {
	static std::string	default_type = "text/plain";
	size_t				dot_pos = file.find_last_of('.');

	if (dot_pos != std::string::npos && dot_pos + 1 < file.length()) {
		std::string											ext = file.substr(dot_pos + 1);
		std::map<std::string, std::string>::const_iterator	it = _mime_container.find(ext);
		if (it != _mime_container.end())
			return it->second;
	}
	return (default_type);
}

bool	Request::isRequestError( void ) const {
	return ( this->_is_error );
}

std::string const&	Request::getCodeError( void ) const {
	return ( this->_error_code);
}

std::string const&	Request::getUploadDir( void ) const {
	return ( this->_upload_dir);
}

std::string const&	Request::getMessageError( void ) const {
	return ( this->_error_message );
}

std::string const&	Request::getVersion( void ) const {
	return ( this->_version );
}

std::string const&	Request::getBody( void ) const {
	return ( this->_body );
}

std::string const&	Request::getBoundary( void ) const {
	return ( this->_boundary );
}

std::string const&	Request::getUploadContent( void ) const {
	return ( this->_upload_content );
}

std::string const&	Request::getFileName( void ) const {
	return ( this->_file_name );
}

std::string const&	Request::getPostContentType( void ) const {
	return ( this->_post_content_type );
}

const std::map<std::string, std::string>& Request::getCookies( void ) const {
	return ( this->_cookies );
}

const std::string& Request::getSessionId( void ) const {
	return ( this->_session_id );
}

void Request::setSessionId( const std::string& id ) {
	this->_session_id = id;
}

static std::string trim_copy( const std::string& s ) {
	size_t	start = 0;
	while (start < s.size() && std::isspace(static_cast<unsigned char>(s[start]))) {
		start++;
	}
	size_t	end = s.size();
	while (end > start && std::isspace(static_cast<unsigned char>(s[end - 1]))) {
		end--;
	}
	return ( s.substr(start, end - start) );
}

void Request::parseCookies( void ) {
	this->_cookies.clear();
	std::map<std::string, std::string>::iterator	it = this->_headers.find("Cookie");
	if (it == this->_headers.end()) {
		return;
	}
	std::string	cookies = it->second;
	size_t		pos = 0;
	while ( pos < cookies.size() ) {
		size_t		semi = cookies.find(';', pos);
		std::string	token;
		if ( semi == std::string::npos ) {
			token = cookies.substr(pos);
			pos = cookies.size();
		} else {
			token = cookies.substr(pos, semi - pos);
			pos = semi + 1;
		}
		token = trim_copy(token);
		if ( token.empty() ) {
			continue;
		}
		size_t eq = token.find('=');
		if ( eq == std::string::npos ) {
			std::string	name = trim_copy(token);
			this->_cookies[name] = "";
		} else {
			std::string	name = trim_copy(token.substr(0, eq));
			std::string	value = trim_copy(token.substr(eq + 1));
			this->_cookies[name] = value;
		}
	}
}
