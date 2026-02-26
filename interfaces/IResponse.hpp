#ifndef IRESPONSE_HPP
# define IRESPONSE_HPP

#include "IRequest.hpp"
#include <iostream>

class IResponse
{
	public:
		virtual ~IResponse() {};
		std::vector<FileFD *>	_fd_files;
		virtual std::string buildPageWithStatusCode(std::string error_code, std::string status, std::string message) = 0;
		virtual void		sendHeader( std::string &body ) = 0;
		virtual void		sendBody( const std::string &body ) = 0;
		virtual void		buildErrorPage( std::string status, std::string error_code, std::string message ) = 0;
		virtual void		sendResponse( std::string &body ) = 0;
		virtual char**		buildEnvp( void ) = 0;
		virtual void		sendReponseInRequestFile( void ) = 0;
		virtual bool		sendResponseWithCgi( IPollFD& poll_event ) = 0;
		virtual void		createFilesResponse( void ) = 0;

		// bool	sendResponse( short type, IPollFD& poll_event );
		virtual bool	sendResponseSimpleFD( int client_fd, IPollFD& poll_event ) = 0;
		virtual bool	sendResponse( int client_fd, short type, IPollFD& poll_event ) = 0;
		virtual short	getResponseType(void) = 0;
		virtual void	setResponseType( short type ) = 0;
	};

#endif
