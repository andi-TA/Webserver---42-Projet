/*
	__REQUEST__HPP__
*/

#ifndef __REQUEST__HPP__
# define __REQUEST__HPP__

# include <map>
# include <cstdlib>
# include <sstream>
# include <fstream>
# include <sys/stat.h>

# include "AFD.hpp"
# include "utils.hpp"
# include "FileFD.hpp"
# include "IPollFD.hpp"
# include "IRequest.hpp"
# include "ServerFD.hpp"

class ServerFD;

# define SUCCES			0
# define NOTFOUND		404
# define FORBIDEN		403
# define INTERNERROR	500

# define URI_LENGTH 1900

typedef struct stat	s_stat;

# define CRLF				"\r\n"
# define CRLFCRLF			"\r\n\r\n"
# define UPLOAD_FILE		"multipart/form-data"
# define POST_FORMULAIRE	"application/x-www-form-urlencoded"

class Request : public IRequest {
	private:

		bool											_is_error;				// Indicates whether the request has an error
		std::string										_error_code;
		std::string										_error_message;
		std::string										_body;
		std::string										_version;
		std::string										_boundary;

		std::string										_post_content_length;	// Size (in bytes) of the POST body sent by the client
		ServerFD*										_server_fd;				// Pointer to the server's file descriptor object
		std::string										_upload_dir;			// Directory where uploaded files are stored
		std::string										_path;					// Path extracted from the request URI
		std::string										_full_path;				// Resolved full filesystem path based on configuration
		std::string										_method;				// HTTP method of the request (e.g., GET, POST)
		std::string										_content_type;			// MIME type of the request body
		std::string										_post_content_type;		// MIME type of the post request body
		std::string										_query_string;			// Query string part of the URI
		const std::string*								_request;				// Pointer to the raw request string
		std::vector< FileFD* >							_upload_file_fd;		// File descriptors to register with the poller
		std::map< std::string, std::string >			_key_value;				// Key-value pairs parsed from POST data
		std::map< std::string, std::string >			_headers;				// Map of HTTP header names to their values
		std::map< std::string, std::string >			_cookies;				// Parsed cookies from Cookie header
		std::string										_session_id;			// Session id parsed from cookies or assigned
		static std::map< std::string, std::string >		_mime_container;		// Static map of file extensions to MIME types

		bool											_is_upload_type;		// True if the request is a multipart/form-data upload
		std::string										_upload_content;		// Raw content of the uploaded part(s)
		std::string										_file_name;				// Filename extracted from the uploaded data
		
		
		
		void	parseHeaders( void );
		void	parseCookies( void );
		
		void	setBody( void );
		void	setPath( void );
		void	setMethod( void );
		void	sliceslash( void );
		void	setVersion( void );
		void	setBoundary( void );
		void	setFullPath( void );
		void	setContentType( void );
		void	setQueryString( void );
		void	resetContentType( void );
		void	setMimeContainer( void );
		void	setPostContentLength( void );
		void	setKeyValueInRequest( void );
		void	setDelete( std::string path, int& status );
		void	setUploadContentAndFileName( size_t pos_begin, size_t pos_end );
		void	setCodeAndMessageError( const std::string & code, const std::string & message );
		
		void	showAll( void );
		void	showBody( void );
		void	showRequest( void );
		void	showCookies( void );
		void	showHeaders( void );
		void	showCTPCTPCL( void );
		void	showKeyValue( void );
		void	showPostHeader( void );
		
		void	checkUploadDir( void );
		void	checkOthersError( void );
		void	checkRequestError( void );
		void	checkAllBadRequest( void );
		
		std::string const&	getVersion( void ) const;
		std::string const&	getBoundary( void ) const;
		std::string const&	getUploadContent( void ) const;
		std::string const&	getFileName( void ) const;
		size_t				getRequestBodySize( void );

		bool	isUploadFile( void );
		bool	isPostFormulaire( void );
		bool	isIncompletRequest( void );
		bool	isBadMethod( void );

		void	setKeyValueWithPostFormulaire( void );

		void	uploadFile( void );

	public:
		Request();
		Request( const Request & other );
		const Request & operator = ( const Request & other );
		Request( int server_fd, IPollFD& allFD, const std::string& request );
		~Request();


		std::string const&							getBody( void ) const;
		std::string const&							getPath( void ) const;
		std::string const&							getMethod( void ) const;
		const std::map<std::string, std::string>&	getCookies( void ) const;
		const std::string&							getSessionId( void ) const;
		std::string const&							getCodeError( void ) const;
		std::string const&							getUploadDir( void ) const;
		bool										isRequestError( void ) const;
		std::string const&							getContentType( void ) const;
		std::string	const&							getQueryString( void ) const;
		std::string const&							getMessageError( void ) const;
		std::string const&							getPostContentType( void ) const;
		std::string const&							getPostContentLength( void ) const;
		const std::map<std::string, std::string>&	getKeyValueInRequest( void ) const;
		std::string const&							getContentTypeWithParam( const std::string& file ) const;


		ServerFD*									getServer( void );
		std::vector< FileFD* >&						getUploadFileFD( void );
		bool										isUploadType( void ) const;
		void										setAll( std::string const& status );
		bool										methodeIsNotImplemented( std::string method );

		void	setSessionId( const std::string& id );

		void	showTrueInfoRequest( void );
		void	uriDecode( std::string& str );
};

#endif //__REQUEST__HPP__
