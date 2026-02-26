/*
	POST Request
*/

#include "Request.hpp"

bool	Request::isUploadFile( void ) {
	return ( this->_content_type.find( UPLOAD_FILE) != std::string::npos );
}

bool	Request::isPostFormulaire( void ) {
	return ( !this->_content_type.compare( POST_FORMULAIRE ) );
}

void	Request::setKeyValueWithPostFormulaire( void ) {
	std::string			body = this->_body;
	this->uriDecode( body );
	std::istringstream	stream( body );
	std::string			key;
	std::string			value;

	while ( std::getline( stream, key, '=' ) && std::getline( stream, value, '&' ) ) {
		key.erase(key.find_last_not_of(" \t") + 1);
		value.erase(0, value.find_first_not_of(" \t"));
		value.erase(value.find_last_not_of(" \t") + 1);
		this->_key_value[key] = value;
	}
}

void	Request::setPostContentLength() {
	if ( this->_method == "POST" ) {
		if (this->_headers.count("Content-Length")) {
			this->_post_content_length = this->_headers["Content-Length"];
		} else {
			this->_post_content_length = "-1";
		}
	} else {
		this->_post_content_length = "0";
	}
}

void	Request::setKeyValueInRequest( void ) {
	this->_key_value.clear();
	if ( this->getMethod().compare( "POST" ) || !this->_headers.count( "Content-Type" ) ) {
		return;
	}
	this->setBody();
	if ( this->isUploadFile() ) {
		if ( !this->_server_fd->getCgiPass( this->_path, ftGetExtensionInFile( this->_path ) ).size() ) {
			this->setBoundary();
			this->uploadFile();
		}
	} else {
		if ( this->isPostFormulaire() ) {
			this->setKeyValueWithPostFormulaire();
		} else {
			this->setCodeAndMessageError( "415", "Unsupported Media Type" );
			this->_is_error = true;
		}
	}
	this->resetContentType();
}

void	Request::uploadFile( void ) {
	bool	is_bad_request = true;
	if ( this->getBoundary().empty() ) {
		return ;
	}
	size_t		pos = 0;

	while ( ( pos = this->_body.find( this->getBoundary(), pos ) ) != std::string::npos ) {
		pos += this->getBoundary().length();
		size_t		end = this->_body.find(this->getBoundary(), pos);
		if ( end == std::string::npos ) {
			break;
		}
		this->setUploadContentAndFileName(pos, end);
		if ( !this->getFileName().empty() && !this->getUploadContent().empty() ) {
			std::string		path = this->_upload_dir +  "/" + this->getFileName();

			int fd = open( path.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0666 );
			if ( fd != -1 ) {

				this->_upload_file_fd.push_back(new FileFD(fd, this->getUploadContent(), POLLOUT));
				this->setCodeAndMessageError("201", "File uploaded");
				this->_is_upload_type = true;
			} else {
				this->setCodeAndMessageError("500", "Internal Server Error: Cannot create file [without CGI]");
			}
			is_bad_request = false;
		} else if ( is_bad_request ) {
			this->setCodeAndMessageError("400", "Bad Request: Upload file error");
		}
	}
}
