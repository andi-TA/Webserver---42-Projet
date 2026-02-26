/*
	eRequest.cpp
*/

#include "Request.hpp"

bool	Request::isBadMethod( void ) {
	for ( size_t i = 0; i < this->getMethod().size(); i ++ ) {
		if ( std::islower( this->getMethod().at(i) ) || !std::isalpha( this->getMethod().at(i) ) )
			return ( true );
	}
	return ( false );
}

bool	Request::isIncompletRequest( void ) {
	size_t headers_end = this->_request->find(CRLFCRLF);

	if ( headers_end == std::string::npos ) {
		return (true);
	}

	if ( this->_method == "POST" ) {
		if ( this->_headers.count("Content-Length") ) {
			std::string len_str = this->_headers["Content-Length"];
			long declared = 0;
			declared = std::strtol(len_str.c_str(), NULL, 10);
			if (declared == 0 && len_str.find_first_not_of('0') != std::string::npos) {
				return (true);
			}
			if ( declared > 0 ) {
				size_t body_pos = headers_end + 4;
				size_t actual_body = 0;
				if ( body_pos < this->_request->size() )
					actual_body = this->_request->size() - body_pos;
				if ( (long)actual_body < declared )
					return (true);
			}
		} else {
			return (true);
		}
	}
	return (false);
}

void	Request::checkAllBadRequest( void ) {
	if ( this->_request->find(CRLF) == std::string::npos ) {
		this->setCodeAndMessageError( "400", "Bad Request: Malformed request line" );
	} else if ( !this->_request || this->_request->empty() ) {
		this->setCodeAndMessageError( "400", "Bad Request: Empty request" );
	} else if ( this->getMethod().empty() ) {
		this->setCodeAndMessageError( "400", "Bad Request: No method" );
	} else if ( this->isBadMethod() ) {
		this->setCodeAndMessageError( "400", "Bad Request: Invalid method" );
	} else if ( this->getPath().empty() ) {
		this->setCodeAndMessageError( "400", "Bad Request: No path" );
	} else if ( this->getVersion().empty() ) {
		this->setCodeAndMessageError( "400", "Bad Request: Empty HTTP version" );
	} else if ( !this->_version.compare( "HTTP/1.1" ) && this->_headers.find( "Host" ) == this->_headers.end() ) {
		this->setCodeAndMessageError( "400", "Bad Request: Host header required" );
	}
}

void	Request::checkOthersError( void ) {
	if ( this->isRequestError() ) {
		return ;
	} else if ( this->getPath().size() < this->_server_fd->getLocationUri( this->getPath() ).size() ) {
		this->setCodeAndMessageError( "403", "Forbidden" );
	} else if ( !this->_server_fd->isAllowed( this->getPath(), this->getMethod() ) ) {
		this->setCodeAndMessageError( "405", "Method Not Allowed" );
	} else if ( !this->getMethod().compare( "POST" ) && this->_post_content_length.at(0) == '-' ) {
		this->setCodeAndMessageError( "411", "Length Required" );
	} else if ( this->_server_fd->getMaxBodySize() < this->getRequestBodySize() && this->_server_fd->getMaxBodySize() ) {
		this->setCodeAndMessageError( "413", "Payload Too Large" );
	} else if ( this->getPath().size() + this->getQueryString().size() > URI_LENGTH ) {
		this->setCodeAndMessageError( "414", "URI Too Long" );
	} else if ( !this->methodeIsNotImplemented( this->getMethod() ) ) {
		this->setCodeAndMessageError( "501", "Not Implemented" );
	} else if ( this->getVersion().compare( "HTTP/1.0" ) && this->getVersion().compare( "HTTP/1.1" ) ) {
		this->setCodeAndMessageError( "505", "HTTP Version Not Supported" );
	}
}

void	Request::checkRequestError( void ) {
	this->_is_error = false;

	this->checkAllBadRequest();
	this->checkOthersError();
	this->checkUploadDir();
}

void	Request::setCodeAndMessageError( const std::string & code, const std::string & message ) {
	this->_is_error = true;
	this->_error_code = code;
	this->_error_message = message;
}

size_t	Request::getRequestBodySize( void ) {
	size_t	body_pos = this->_request->find( CRLFCRLF );
	if ( body_pos == std::string::npos ) {
		return ( 0 );
	}
	std::string	body = this->_request->substr( body_pos + 4 );
	return ( body.size() );
}

void	Request::checkUploadDir( void ) {
	s_stat		file_info;

	if (this->isRequestError())
		return ;
	if ( this->getMethod().compare( "POST" ) || this->getContentType().find( UPLOAD_FILE ) == std::string::npos ) {
		return ;
	}
	if ( this->getUploadDir()[0] != '.' ) {
		this->_upload_dir = "." + this->getUploadDir();
	}
	if ( stat((this->getUploadDir()).c_str(), &file_info) == -1 || this->getUploadDir().empty() ) {
		this->setCodeAndMessageError( "404", "Upload dir not found" );
	} else if (S_ISREG(file_info.st_mode)) {
		this->setCodeAndMessageError( "500", "Internal Server Error : Upload dir must be a directory" );
	} else if (S_ISDIR(file_info.st_mode)) {
		if (access(this->getUploadDir().c_str(), W_OK) == -1) {
			this->setCodeAndMessageError( "403", "Forbidden : No write access to upload dir" );
		}
	}
}
