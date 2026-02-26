/* 
	setRequest.cpp
 */

#include "Request.hpp"

void	Request::setMethod( void ) {
	size_t	first_space = this->_request->find(' ');

	if ( first_space != std::string::npos ) {
		this->_method = this->_request->substr(0, first_space);
	} else {
		this->_method = "";
	}
}

void	Request::setPath( void ) {
	size_t	first_space = this->_request->find(' ');

	if (first_space == std::string::npos) {
		this->_path = "";
		return;
	}

	size_t	second_space = this->_request->find(' ', first_space + 1);

	if (second_space == std::string::npos) {
		this->_path = this->_request->substr(first_space + 1);
		return;
	}
	this->_path = this->_request->substr(first_space + 1, second_space - first_space - 1);

	size_t query_pos = this->_path.find('?');

	if (query_pos != std::string::npos) {
		this->_path = this->_path.substr(0, query_pos);
	}

	this->uriDecode( this->_path );
	WebServer::sliceSlash( this->_path );
	this->_upload_dir = this->_server_fd->getUploadDir(this->_path);
}

void	Request::setContentType() {
	if ( this->_method == "GET" ) {
		size_t		dot_pos = this->_path.find_last_of('.');
		size_t		location_pos = this->_path.find( this->_server_fd->getLocationUri( this->_path ) );
		std::string	real_path = this->_path;

		if ( location_pos != std::string::npos ) {
			real_path = "/" + this->_path.substr( this->_server_fd->getLocationUri( this->_path ).size() );
		}

		if ( !ftUriIsDir( real_path, this->_server_fd->getRoot( this->_path ) ) ) {
			if (dot_pos != std::string::npos) {
				this->_content_type = this->getContentTypeWithParam( this->_path );
			} else {
				this->_content_type = this->getContentTypeWithParam( "default-ext" );
			}
		} else {
			if ( this->_server_fd->autoIndexON( this->_path ) ) {
				this->_content_type = this->getContentTypeWithParam( "ext.html" );
			} else {
				this->_content_type = this->getContentTypeWithParam( this->_server_fd->getIndex( this->_path ) );
			}
		}
	} else {
		if (this->_headers.count("Content-Type")) {
			this->_content_type = this->_headers["Content-Type"];
		} else {
			this->_content_type = this->getContentTypeWithParam( "default-ext" );
		}
	}
}

void	Request::setMimeContainer( void ) {
	this->_mime_container.insert( std::make_pair( "c", "text/x-c" ) );
	this->_mime_container.insert( std::make_pair( "css", "text/css" ) );
	this->_mime_container.insert( std::make_pair( "csv", "text/csv" ) );
	this->_mime_container.insert( std::make_pair( "htm", "text/html" ) );
	this->_mime_container.insert( std::make_pair( "png", "image/png" ) );
	this->_mime_container.insert( std::make_pair( "mp4", "video/mp4" ) );
	this->_mime_container.insert( std::make_pair( "gif", "image/gif" ) );
	this->_mime_container.insert( std::make_pair( "bmp", "image/bmp" ) );
	this->_mime_container.insert( std::make_pair( "wav", "audio/wav" ) );
	this->_mime_container.insert( std::make_pair( "php", "text/html" ) );
	this->_mime_container.insert( std::make_pair( "ogg", "audio/ogg" ) );
	this->_mime_container.insert( std::make_pair( "ogv", "video/ogg" ) );
	this->_mime_container.insert( std::make_pair( "cpp", "text/x-c++" ) );
	this->_mime_container.insert( std::make_pair( "html", "text/html" ) );
	this->_mime_container.insert( std::make_pair( "mp3", "audio/mpeg" ) );
	this->_mime_container.insert( std::make_pair( "jpg", "image/jpeg" ) );
	this->_mime_container.insert( std::make_pair( "txt", "text/plain" ) );
	this->_mime_container.insert( std::make_pair( "jpeg", "image/jpeg" ) );
	this->_mime_container.insert( std::make_pair( "webm", "video/webm" ) );
	this->_mime_container.insert( std::make_pair( "avif", "image/avif" ) );
	this->_mime_container.insert( std::make_pair( "flac", "audio/flac" ) );
	this->_mime_container.insert( std::make_pair( "midi", "audio/midi" ) );
	this->_mime_container.insert( std::make_pair( "py", "text/x-python" ) );
	this->_mime_container.insert( std::make_pair( "ico", "image/x-icon" ) );
	this->_mime_container.insert( std::make_pair( "md", "text/markdown" ) );
	this->_mime_container.insert( std::make_pair( "svg", "image/svg+xml" ) );
	this->_mime_container.insert( std::make_pair( "svgz", "image/svg+xml" ) );
	this->_mime_container.insert( std::make_pair( "pdf", "application/pdf" ) );
	this->_mime_container.insert( std::make_pair( "sh", "application/x-sh" ) );
	this->_mime_container.insert( std::make_pair( "xml", "application/xml" ) );
	this->_mime_container.insert( std::make_pair( "zip", "application/zip" ) );
	this->_mime_container.insert( std::make_pair( "gz", "application/gzip" ) );
	this->_mime_container.insert( std::make_pair( "avi", "video/x-msvideo" ) );
	this->_mime_container.insert( std::make_pair( "mov", "video/quicktime" ) );
	this->_mime_container.insert( std::make_pair( "json", "application/json" ) );
	this->_mime_container.insert( std::make_pair( "wasm", "application/wasm" ) );
	this->_mime_container.insert( std::make_pair( "tar", "application/x-tar" ) );
	this->_mime_container.insert( std::make_pair( "rar", "application/vnd.rar" ) );
	this->_mime_container.insert( std::make_pair( "js", "application/javascript" ) );
	this->_mime_container.insert( std::make_pair( "7z", "application/x-7z-compressed" ) );
}

void	Request::setQueryString( void ) {
	size_t	first_space = this->_request->find(' ');
	if (first_space == std::string::npos) {
		this->_query_string = "";
		return;
	}

	size_t	second_space = this->_request->find(' ', first_space + 1);
	if (second_space == std::string::npos) {
		this->_query_string = "";
		return;
	}

	std::string	path_with_query = this->_request->substr(first_space + 1, second_space - first_space - 1);
	size_t		query_pos = path_with_query.find('?');
	if (query_pos != std::string::npos) {
		this->_query_string = path_with_query.substr(query_pos + 1);
	} else {
		this->_query_string = "";
	}
}

void	Request::setFullPath( void ) {
	size_t		location_pos = this->_path.find( this->_server_fd->getLocationUri( this->_path ) );
	std::string	real_path = this->_path;

	if ( location_pos != std::string::npos ) {
		real_path = "/" + this->_path.substr( this->_server_fd->getLocationUri( this->_path ).size() );
	}
	this->_full_path = this->_server_fd->getRoot( this->getPath() ) + real_path;
	if ( this->_full_path[0] != '.' )
		this->_full_path = "." + this->_full_path;
}

void	Request::setVersion( void ) {
	size_t	end_line = this->_request->find(CRLF);
	if ( end_line == std::string::npos ) {
		this->_version = "";
		return ;
	}

	size_t	second_space = this->_request->rfind(' ', end_line);
	this->_version = this->_request->substr( second_space + 1, end_line - second_space - 1 );
}

void	Request::setBody( void ) {
	this->_body = "";
	size_t	body_pos = this->_request->find( CRLFCRLF );
	if ( body_pos != std::string::npos ) {
		this->_body = this->_request->substr( body_pos + 4 );
	}
}

void	Request::setBoundary( void ) {
	this->_boundary = "";
	size_t	eq_pos = this->_content_type.find('=');
	if ( eq_pos != std::string::npos ) {
		this->_boundary = "--" + this->_content_type.substr( eq_pos + 1 );
	}
}

void	Request::resetContentType( void ) {
	std::string	real_path = this->_path;
	size_t		location_pos = this->_path.find( this->_server_fd->getLocationUri( this->_path ) );

	if ( location_pos != std::string::npos ) {
		real_path = "/" + this->_path.substr( this->_server_fd->getLocationUri( this->_path ).size() );
	}
	this->_post_content_type = this->_content_type;
	if ( ftUriIsDir( real_path, this->_server_fd->getRoot( this->_path ) ) ) {
		this->_content_type = this->getContentTypeWithParam( this->_server_fd->getIndex( this->_path ) );
	} else {
		this->_content_type = this->getContentTypeWithParam( this->_full_path );
	}
}

void	Request::setUploadContentAndFileName( size_t pos_begin, size_t pos_end ) {
	this->_upload_content = this->_body.substr( pos_begin, pos_end - pos_begin );
	size_t		filename_pos = this->_upload_content.find("filename=\"");
	if ( filename_pos != std::string::npos ) {
		size_t			filename_end = this->_upload_content.find("\"", filename_pos + 10);
		this->_file_name = this->_upload_content.substr(filename_pos + 10, filename_end - (filename_pos + 10));
	} else {
		this->_file_name = "";
	}

	size_t		content_pos = this->_upload_content.find( CRLFCRLF );
	if ( content_pos != std::string::npos ) {
		this->_upload_content = this->_upload_content.substr( content_pos + 4 );
	} else {
		this->_upload_content = "";
	}
}
