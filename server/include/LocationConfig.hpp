#ifndef LOCATIONCONFIG_HPP
# define LOCATIONCONFIG_HPP

#include <iostream>
#include <vector>
#include <map>
#include "WebServer.hpp"

class WebServer;

class LocationConfig{
	public:
		LocationConfig( void );
		LocationConfig( std::string const& uri );
		LocationConfig( const LocationConfig & cpy );
		~LocationConfig( void );
		LocationConfig&	operator=( const LocationConfig & cpy );
	
		/**
		 * 
		 * 					PUBLIC	METHODS
		 * 
		 * 	->	Set one bloc of location instance		[ setLocationBloc ]
		 * 	->	diplay the location attribut contents  	[ display() ]
		 * 	->	Reset the location attribut contents	[ clear() ]
		 * 	->	attribut contents getters				[ set...() ]
		 * 
		 */
		
		void	setLocationBloc( std::vector< std::vector<std::string> >::iterator&   it_serv );
		void	clear( void );

	 	static bool			isLocationWordKey( const std::string& str );
	
		const std::string&	getCgiPass( std::string const& extension );
		const std::string&	getRedirectPath( void );
		const std::string&	getRedirectCode( void );
		const std::string&	getUploadDir( void );
		const std::string&	getIndex( void );
		const std::string&	getRoot( void );
		const std::string&	getUri( void );
		bool				isAllowedMethod( const std::string& method );
		bool				getAutoIndex( void );
		bool				getCookiesSupport( void );

		/* ATTRIBUTS */
		

	private:
		

		/**
		 * 
		 * 						PRIVATE	METHODS
		 * 
		 * 	->	attribut contents setters				[ set...() ]
		 *
		 */
		
		void	setRedirection( std::vector<std::string>& content );
		void	setAutoIndex( std::vector<std::string>& content );
		void	setCookiesSupport( std::vector<std::string>& content );
		void	setUploadDir( std::vector<std::string>& content );
		void	setMethods( std::vector<std::string>& content );
		void	setIndex( std::vector<std::string>& content );
		void	setRoot( std::vector<std::string>& content );
		void	setCgi( std::vector<std::string>& content );
		

		/**
		 * 
		 * 						PRIVATE	ATTRIBUTS
		 * 
		 * 	->	_methodStat		:	allowd method
		 * 	->	_full_path		:	root + uri
		 * 	->	_root			:	root_path
		 * 	->	_uri			:	uri location
		 * 	->	_index			:	default file
		 * 	->	_cgi_pass		:	cgi
		 * 	->	_autoindex		:	enable or disable listing
		 * 	->	_required_key	:	to check existing location key in config file
		 * 
		 */
		std::map<std::string, bool>	_required_key; 

		std::map<std::string, std::string>	_all_cgi;	 
		std::map<std::string, bool>			_methodStat;	 
		std::string							_full_path;
		std::string							_redirect_code;
		std::string							_redirect_path;
		std::string							_upload_dir;
		std::string							_index;
		std::string							_root;
		std::string							_uri;
		std::string							_tmp;
		bool								_autoindex;
		bool								_cookies_support;

};

#endif
