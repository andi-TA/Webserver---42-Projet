#ifndef SERVERFD_HPP
# define SERVERFD_HPP

#include "LocationConfig.hpp"
#include "AFD.hpp"

#define PORT_MAX	65535
#define PORT_MIN	1024

class LocationConfig;

class WebServer;

class AFD;

class ClientFD;

/* ****************** HERIT FROM AFD CLASS ***************** */

class ServerFD : public AFD {
	public:
		ServerFD( void );
		ServerFD( const ServerFD & cpy );
		~ServerFD( void );
		
		/**
		 *
		 *  					PUBLIC	METHODS
		 * 
		 * 	->	set an Server instance from one server bloc		[ setServerBloc() ]
		 * 	->	insert one location bloc in the server			[ insertLocationBlock() ]
		 * 	->	display the server contents						[ display() ]
		 *  ->	the getter methods for each server attributs	[ get...() ]
		 * 
		 */

		void	setServerBloc( std::vector< std::vector<std::string> >::iterator&   it_serv );
		void	insertLocationBlock( std::vector< std::vector<std::string> >::iterator&   it_serv );
		
		std::vector<LocationConfig>	getLocations( void ) const;
        std::string const& 			getFDType( void ) const;
		std::string const&			getHost( void ) const;
		std::string const&			getPort( void ) const;
		size_t						getRequestSize( void ) const;
		
		int&						getFD( void ) ;


		
		
						//	collab functions
		std::map<std::string, std::string> const&	getErrorIndex( void ) const;
		std::string const&							getLocationUri( std::string const& uri );
		std::string const&							getRedirectPath( std::string const& uri );
		std::string const&							getRedirectCode( std::string const& uri );
		std::string const&							getCgiPass( std::string const& uri, std::string const& extension );
		std::string const&							getIndex( std::string const& uri ) ;
		std::string const&							getRoot( std::string const& uri );
		std::string const&							getSimpleUploadDir( std::string const& uri );
		std::string const							getUploadDir( std::string const& uri );
		bool										isAllowed( std::string const& uri , std::string const& method );
		void										handleEvent( IPollFD& poll_fd );
		bool										autoIndexON( std::string const& uri );
		bool										cookiesSupportON( std::string const& uri );
		size_t										getMaxBodySize( void ) const;			// [Naval]
		
		static bool									isServerWordKey( const std::string& str );

		std::vector<std::string>&					getMultiPort( void ) ;
		std::string const&							getPort( void ) ;
		std::string const&							getHost( void ) ;

		void										setHost( std::string const& port );
		void										setPort( std::string const& port );

		void										addClient( ClientFD* new_client );
		ClientFD*									getClientByFD( int fd );
		std::map<int, ClientFD*>&					getAllClient( void );

		void										setFD( int fd );
		
	private:

		ServerFD&	operator=( const ServerFD & cpy );

		/**
		 * 
		 * 						PRIVATE	METHODS
		 * 
		 * 	->	the setter methods of each server attributs				[ set...() ]
		 * 	->	bool function to know if word is a server directive 	[ isServerWordKey() ]
		 * 	->	reset server content						  			[ clear() ]
		 * 
		 */
		
		std::string const	searchRightUri( std::string const& uri );
		short				getEvents( void ) const;
		void				setMaxBodySize( std::vector<std::string>& content );
		void				setErrorPage( std::vector<std::string>& content );
		void				setMultiPort( std::vector<std::string>& content );

		void				clear( void );
		
		
		/**
		 *
		 *  						PRIVATE	ATTRIBUTS
		 * 	->  _required_key			:	to check existing server key in config file
		 *  ->	_error_page				:	error_pages path + file
		 *	->	_locations				:	list of all locations block
		 *	->	_server_name			:	Server name "host"
		 *	->	_port					:	port to listen
		 *	->	_request_body_size	:	max number of request queue
		 *	->	_max_body_size		:	max number of body allowed		[Naval]
		 *
		 */
		std::map<std::string, bool> _required_key;		  

		std::map<std::string, std::string>	_error_page; 
		std::vector<LocationConfig>			_locations;
		std::vector<std::string>			_multiple_port;
		std::string							_right_location;
		std::string							_redirect;
		std::string							_host;
		std::string							_port;
		std::string							_tmp;
		int									_request_body_size;
		size_t								_max_body_size;

		std::map<int, ClientFD*>			_clients;

};

#endif
