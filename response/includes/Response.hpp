#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include "IRequest.hpp"
# include "IResponse.hpp"
# include "IServer.hpp"
# include "IConfig.hpp"
# include "IPollFD.hpp"
# include "Request.hpp"
# include "AFD.hpp"
# include "utils.hpp"
# include "FileFD.hpp"

# include <sys/stat.h>
# include <fcntl.h>
# include <dirent.h>
# include <unistd.h>
# include <sys/wait.h>
# include <cstring>
# include <map>
# include <algorithm>
# include <cstdlib>
# include <filesystem>

# define CRLF_END 						"\r\n\r\n"
# define CRLF_END_PYTHON 				"\n\0"
# define STR_SIZE 						1024
# define URI_LENGTH 					1900
# define RESPONSE_SIMPLE_HTML_FD 		1
# define RESPONSE_WITH_CGI		 		2
# define RESPONSE_DIRLIST        		3
# define RESPONSE_ERROR		     		4
# define RESPONSE_UPLOAD_WITHOUT_CGI   	5
# define RESPONSE_REDIRECT				6

# define CGI_STEP_ONE					10
# define CGI_STEP_TWO					12
# define CGI_STEP_THREE					13
# define CGI_STEP_FOUR					14
# define CGI_STEP_FIVE					15
# define CGI_STEP_SIX					16
# define CGI_WAITING_STEP				19

typedef struct stat	s_stat;
typedef struct dirent	s_dirent;


class Response : public IResponse
{
	private:
		int									_pid;
		int									_fd_client;
		int									_server_fd;
		bool								_header_sent;
		bool								_auto_index;
		bool								_support_cookies;
		bool								_is_allowed;
		bool								_request_have_error;
		bool								_get_body;
		short								_type_response_to_send;
		short								_cgi_step_stat;		
		char**								_envp;
		Request*							_request;
		std::string							_tmp;
		std::string							_response_body;
		std::string							_cgi_ext;
		std::string							_upload_dir;
		std::string							_body;
		std::string							_root;
		std::string							_index;
		std::string							_cgi;
		std::string							_path;
		std::string							_method;
		std::string							_content_type;
		std::string							_redirect_uri;
		std::string							_redirect_uri_code;
		std::string							_server_name;
		std::string							_server_port;
		std::string							_location_by_uri;
		std::string							_status;
		std::string 						_error_code;
		std::string							_queryString;
		std::string							_request_error_code;
		std::string							_request_message_error;
		std::map<std::string, std::string>	_error_index;
		
		int			getFdFile( std::string path );

		bool 		fileIsGood( std::string uri, std::string bin_path, bool second_parameter_is_binary, bool full_path );
		bool 		checkRequestIfGood( void );
		bool		methodIsImplement( void );
		bool		rootRequestMatchWithConfigRoot( void );
		bool		fullPathWithRoot(void);
		bool		methodeIsAccespt( void );
		bool		pathIsFull( void );
		bool		redirectionIsGood( void );
		bool		loopRedirectionDetected( std::string location_sub );
		bool		redirectionIsOkWithUri( std::string uri );
		bool		moreVerificationRedirectIsTrue( void );
		bool		writeInPipe( void );
		bool		readInPipe( void );
		bool		actionIsDelete( void );
		bool		redirectExternalHttp( void );

		void		dirList( void );
		void		sendResponseWithoutDirList( void );
		void		sendResponseWithMethodDelete( void );
		void		setFdPipeWithCgi( void );
		void		checkIfDirOrNot( void );
		void		redirectResponse( void );
		void		sendPageMyErrorPage ( std::string &path, std::string &error_code, std::string &status, std::string &message );

		std::string	getRealPathWitUri( std::string& uri );
		std::string getContentForm(std::string request);
		std::string getResponseInRedirectStatusCode( std::string code );
		std::string getContentFile( int fd );
		std::string	getRealPathWithUri( void );

		std::vector<std::string>	_set_cookies;

	public:

		std::vector<FileFD *>	_fd_files;

		Response( void );
		~Response(void);
		Response( Response const & cpy );
		Response & operator=( Response const & cpy );
		Response( int server_fd, int client_fd, IPollFD& allFD, Request& request );

		bool		sendResponseWithCgi( IPollFD& poll_event );
		bool		sendResponseSimpleFD( int client_fd, IPollFD& poll_event );
		bool		sendResponseSimpleUpload( IPollFD& poll_event );
		bool		sendResponse( int client_fd, short type, IPollFD& poll_event );
		bool		SendResponseStr( IPollFD& poll_event );

		void		buildErrorPage( std::string status, std::string error_code, std::string message );
		void		sendBody( const std::string &body );
		void		sendResponse( std::string &body );
		void		sendReponseInRequestFile( void );
		void		sendHeader( std::string &body );
		void		createFilesResponse( void );
		void		setResponseType( short type );
		short		getResponseType(void);
		char**		buildEnvp( void );

		pid_t		getChildPid( void );

		std::string buildPageWithStatusCode(std::string error_code, std::string status, std::string message);
		std::string	getErrorIndex( std::string error_code );

		void		addSetCookie(const std::string& cookie_header_value);

		std::string getPath( void );
		short		getAction( void );
		void		killPid( void );
	};

#endif
