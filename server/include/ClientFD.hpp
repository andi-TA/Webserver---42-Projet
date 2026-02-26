#ifndef CLIENTFD_HPP
# define CLIENTFD_HPP

# include "IResponse.hpp"
# include "AFD.hpp"
# include "IRequest.hpp"
# include "FileFD.hpp"
#include "Request.hpp"
#include "Response.hpp"


# define GETTING_REQUEST 	1000
# define SETTING_REQUEST 	1004
# define SETTING_FILEFD		1001
# define SENDING_RESPONSE	1002
# define NONE				1003

/* ****************** HERIT FROM AFD CLASS ***************** */


class ClientFD : public AFD {
	public:
		ClientFD( void );
		ClientFD( int const& client_fd, int const& server_fd );
		ClientFD( const ClientFD & cpy );
		~ClientFD( void );
	
		ClientFD&	operator=( const ClientFD & cpy );
		
		const std::string&		getErrorCode( void );
		const std::string&		getFDType( void ) const;
		std::string&			getBuffer( void );
		int&					getFD( void );
		short					getEvents( void ) const;
		void					updateLastTimeActivity( void );
		void					setFD( int fd );
		
		int						getServerFd( void );
		
		bool					shouldClose( void );
		
		
		void					handleEvent( IPollFD& poll_event );
		
		Response&				getResponse( void );
		void					setResponse( const Response & rsp );
		
		Request&				getRequest( void );
		void					setRequest( const Request & rqst );
				
		
		bool					isCompleteRequest( std::string const& buffer );
		bool					getRequestString( ClientFD*	client );
		
		void					deleteFilesResponse( IPollFD& poll_event );
		
		bool					isTimeout( time_t timeout );

	private:

		void					setCloseYes( void );
		bool					tooLargeContentLength( std::string const& cl, IPollFD& poll_event );
		std::string				checkRequestError( IPollFD& poll_event);


		//	attributs

		std::string				_error_code;

		std::string				_buffer;
		Response				_response;
		Request					_request;
		
		time_t					_last_time_activity;
		int						_server_fd;
		int						_fd_number;

		short					_action;
		size_t					_content_length;


	};
	
#endif

