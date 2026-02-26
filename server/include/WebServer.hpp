

/**
 * WebServer class is an abstract class where the server main loop run
 *  ->  read the conf file  ->	getAllServer( std::string const& file_name );
 */



#ifndef WEBSERVER_HPP
# define WEBSERVER_HPP

//	C++ lib
# include <exception>
# include <algorithm>
# include <iostream>
# include <fstream>
# include <sstream>
# include <vector>

//	C lib
# include <string.h>
# include <poll.h>

//	Headers
# include "AFD.hpp"
# include "ServerFD.hpp"
# include "PollFD.hpp"


# ifndef ESC
#  define ESC "\033"
# endif

// Reset / normal
# define RESET			ESC "[0m"

// Styles
# define STYLE_BOLD			ESC "[1m"
# define STYLE_DIM			ESC "[2m"
# define STYLE_UNDERLINE	ESC "[4m"
# define STYLE_REVERSED		ESC "[7m"

//	 Standard foreground colors
# define BLACK			ESC "[30m"
# define RED			ESC "[31m"
# define GREEN			ESC "[32m"
# define YELLOW			ESC "[33m"
# define BLUE			ESC "[34m"
# define MAGENTA		ESC "[35m"
# define CYAN			ESC "[36m"
# define WHITE			ESC "[37m"

// Bright foreground colors
# define BRIGHT_BLACK		ESC "[90m"
# define BRIGHT_RED			ESC "[91m"
# define BRIGHT_GREEN		ESC "[92m"
# define BRIGHT_YELLOW		ESC "[93m"
# define BRIGHT_BLUE		ESC "[94m"
# define BRIGHT_MAGENTA		ESC "[95m"
# define BRIGHT_CYAN		ESC "[96m"
# define BRIGHT_WHITE		ESC "[97m"

// Background colors
# define BG_BLACK		ESC "[40m"
# define BG_RED			ESC "[41m"
# define BG_GREEN		ESC "[42m"
# define BG_YELLOW		ESC "[43m"
# define BG_BLUE		ESC "[44m"
# define BG_MAGENTA		ESC "[45m"
# define BG_CYAN		ESC "[46m"
# define BG_WHITE		ESC "[47m"

//	utils
# define COOKIES_SUPPORT	"cookies_support"
# define MAX_BODY_SIZE		"max_body_size"
# define HTTP_METHODS		"allow_methods"
# define SERVER_NAME		"server_name"
# define ERROR_PAGE			"error_page"
# define UPLOAD_DIR			"upload_dir"
# define AUTOINDEX			"autoindex"
# define REDIRECT			"return"
# define LOCATION			"location"
# define CLIENT				"client"
# define SERVER				"server"
# define INDEX 				"index"
# define FILE 				"file" 
# define PORT 				"listen"
# define ROOT 				"root"
# define CGI 				"cgi_pass"

class ServerFD;

class PollFD;

class WebServer{
	public:
		~WebServer( void );
		
		/**
		 * 
		 * 							PUBLIC	METHODS
		 * 
		 *  ->  get all of the server blocs content 			[ getAllServer() ]
		 * 	->	identify directive delimiter : ';', '{', '}' 	[ cleanConfigContent() ]
		 * 	->	get one directive word length					[ directiveLen() ]
		 * 	->	remove next servers that had the same host:port	[ removeDuplicates() ]
		 * 	->	launch all of the servers						[ launchServer() ]
		 *	->	delete all of the AFD* vector            		[ freeServer() ]
		 * 
		 */
		static std::vector<ServerFD*>&	getServerTab( void );
		static std::string&				cleanConfigContent( std::string& content );
		static size_t					directiveLen( std::vector<std::string>& content );
		static void						getAllServer( std::string const& file_name, PollFD* poll_event );
		static void						sliceSlash( std::string& path );
		static void						removeMultiPort( void );
		static void						splitServer( void );
		static void						launchServer( void );
		static void						freeServer( void );

		static void						closeFD( int& fd );
		static void						display( int fd, std::string const& mssg, std::string const& color);

	private:
		/**
		 * 
		 * 							PRIVATE METHODS
		 * 
		 *	->	insert all of the server directives in a vector	[ getAllBlock() ]
		 *	->	check if there is still a word after ';'		[ existWord() ]
		 * 
		 */

		static std::vector< std::vector< std::string > >	getAllBlock( std::string& file_content );
		static bool											existWord( std::string const& str, size_t begin );


		/**
		 * 
		 * 							PRIVATE ATTRIBUT
		 * 
		 * 	->	_server_config			:	vector of AFD class 	
		 */
		static std::vector< ServerFD* >								_server_config;
		static PollFD*												_poll_event;
		static std::string											_delim[3];
		static std::string											_tmp;
		static std::vector< std::vector< std::string > >::iterator	_it_vec;
		static std::vector< std::vector< std::string > >			_vec_vec;
		static std::vector< std::string >::iterator					_it_str;
		static std::vector< ServerFD* >::iterator					_it_serv;
		static std::vector< std::string >							_vec;
		static std::vector< ServerFD* >								_vec_serv;

};

#endif
