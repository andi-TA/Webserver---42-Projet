#ifndef POLLFD_HPP
# define POLLFD_HPP

#include "IPollFD.hpp"
#include "AFD.hpp"
#include "WebServer.hpp"

class AFD;

class PollFD : public IPollFD 
{
	public:
		/**
		 * 
		 * 			 PUBLIC METHODS 
		 * 
		 */

		PollFD( std::vector<ServerFD*>& afds );
		PollFD( const PollFD & cpy );
		PollFD( void );
		PollFD&	operator=( const PollFD & cpy );
		~PollFD( void );
	
		void	update( void );

		std::vector<ServerFD*>&		getAllServer( void );
		std::vector<struct pollfd>&	getAllPollFD( void );

		//	Getters
		struct pollfd&		getPollFD( int i ) ;
		ServerFD*			getServer( int fd ) ;
	
		short				getFdRevent( int fd );

		void				addPollFd( int fd, short event );

		void				setFdEvent( int fd, short event );

		void				deletePollFD( int fd );

		const std::string&	getConfigFile( void );
		void				setConfigFile( const std::string& filename );


	private:
		/**
		 * 
		 * 			PRIVATE METHODS
		 * 
		 * ->	Coplien
		 * 
		 */

		std::vector<struct pollfd>	_poll_fds;
		std::vector<ServerFD*>		_all_servers;

		std::string					_config_filename;

		//	Attribut
};

#endif
