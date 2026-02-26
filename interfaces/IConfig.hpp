#ifndef ICONFIG_HPP
#define ICONFIG_HPP

#include <iostream>

class IConfig
{
	public:
		virtual						~IConfig() {};
		virtual std::string const&	getRoot( std::string const& uri ) const = 0;
		virtual std::string const&	getIndex( std::string const& uri ) const = 0;
		virtual	std::string const&	getErrorIndex( int error ) const = 0;
		virtual	std::string const&	getLocationUri( std::string const& uri ) = 0;
		virtual bool				autoIndexON( std::string const& uri ) const = 0;
		virtual bool				cookiesSupportON( std::string const& uri ) const = 0;
		virtual bool				isAllowed( std::string const& method ) const = 0;
		virtual int					getRequestQueue( void ) const = 0;

};

#endif
