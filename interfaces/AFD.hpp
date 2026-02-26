#ifndef AFD_HPP
# define AFD_HPP

//	C++ lib
# include <iostream>
# include <sstream>
# include <cstring>

//	C lib
# include <poll.h>
# include <sys/types.h>
# include <sys/socket.h>
# include <netdb.h>
# include <unistd.h>
# include <fcntl.h>
# include <stdio.h>


# include "IPollFD.hpp"

# define BUFSIZZ			10000000


class PollFD;

class AFD {
	public:
		virtual ~AFD( void );
		
		/**
		 *
		 *						PUBLIC METHODS
		 * 
		 */
		


		
		//	Redirection functions
		virtual std::string const&	getRedirectPath( std::string const& uri );
		virtual std::string const&	getRedirectCode( std::string const& uri );

		//	Server functions
		virtual std::string const&	getIndex( std::string const& uri );
		virtual std::string const&	getLocationUri( std::string const& uri );
		virtual std::string const&	getRoot( std::string const& uri );
		virtual std::string const&	getErrorIndex( int error ) const;
		virtual std::string const&	getServerName( void ) const;
		virtual std::string const&	getPort( void ) const;
		virtual size_t				getRequestSize( void ) const;
		virtual bool				isAllowed( std::string const& uri , std::string const& method ) ;
		virtual bool				autoIndexON( std::string const& uri );
		virtual bool				cookiesSupportON( std::string const& uri );
		virtual size_t				getMaxBodySize( void ) const;
		
		//	Client functions
		virtual bool				shouldClose( void );
		virtual void				setCloseYes( void );


		//	DISPLAY SERVER BLOC
		virtual void	    		display( void );
		
		virtual void				writeFD( std::string const& str );


		//	SERVER FD STATS
        virtual std::string const&	getFDType( void ) const = 0;
		virtual short				getEvents( void ) const = 0;
		virtual	int&				getFD( void ) = 0;
		virtual void				handleEvent( IPollFD& poll_event );
		virtual	void				setFD( int fd ) = 0;
		

    protected:
		std::string					_type;
		short						_event;
		bool						_should_close;
		int							_fd;
};

#endif
