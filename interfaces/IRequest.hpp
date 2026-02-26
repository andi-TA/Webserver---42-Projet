#ifndef IREQUEST_HPP
# define IREQUEST_HPP

#include <iostream>
#include <map>
#include <vector>
#include "FileFD.hpp"

class IRequest
{
	public:
		virtual	~IRequest() {};
		virtual	std::string const &							getPath( void ) const = 0;
		virtual	std::string const &							getMethod( void ) const = 0;
		virtual	std::string const &							getCodeError( void ) const = 0;
		virtual	bool										isRequestError( void ) const = 0;
		virtual	std::string const &							getContentType( void ) const = 0;
		virtual std::string const &							getPostContentType( void ) const = 0;
		virtual	std::string const &							getMessageError( void ) const = 0;
		virtual	std::string const&							getPostContentLength( void ) const = 0;
		virtual	const std::map<std::string, std::string>&	getKeyValueInRequest( void ) const = 0;
		virtual	std::string	const &							getQueryString( void ) const = 0;
		virtual	std::string const &							getContentTypeWithParam( const std::string& file ) const = 0;

		virtual bool										isUploadType( void ) const = 0;
		virtual std::vector< FileFD* >&						getUploadFileFD( void ) = 0;
		virtual void										showRequest( void ) = 0;
		virtual void										setAll( std::string const& status ) = 0;









};

#endif