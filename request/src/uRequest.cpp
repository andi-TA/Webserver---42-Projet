/*
	uRequest.cpp
*/

#include "Request.hpp"

void	Request::parseHeaders( void ) {
	this->_headers.clear();
	size_t	pos = this->_request->find(CRLF) + 2;

	while ( pos < this->_request->size() ) {
		size_t	end = this->_request->find(CRLF, pos);
		if (end == std::string::npos || end == pos)
			break;

		std::string	line = this->_request->substr(pos, end - pos);
		size_t		sep = line.find(':');

		if ( sep != std::string::npos ) {
			std::string	key = line.substr(0, sep);
			std::string	value = line.substr(sep + 1);

			key.erase(0, key.find_first_not_of(" \t"));
			key.erase(key.find_last_not_of(" \t") + 1);
			value.erase(0, value.find_first_not_of(" \t"));
			value.erase(value.find_last_not_of(" \t") + 1);
			this->_headers[key] = value;
		}
		pos = end + 2;
	}
	this->parseCookies();
}

bool	Request::methodeIsNotImplemented( std::string method ) {
	std::string	methods[] = {
			"POST",
			"GET",
			"DELETE",
			""
		};
	int	i = 0;
	while (!methods[i].empty()) {
		if (!methods[i].compare(method))
			return (true);
		i++;
	}
	return (false);
}

void	Request::uriDecode( std::string& str ) {
	std::string	decoded = "";

	for ( size_t i = 0; i < str.length(); ++i ) {
		if ( str[i] == '%' && i + 2 < str.length() ) {
			std::string hex = str.substr(i + 1, 2);
			decoded += static_cast<char>( std::strtol( hex.c_str(), NULL, 16 ) );
			i += 2;
		} else if ( str[i] == '+' ) {
			decoded += ' ';
		} else {
			decoded += str[i];
		}
	}
	str = decoded;
}

void	Request::setDelete( std::string path, int& status ) {
	if ( this->_method.compare( "DELETE") )
		return ;
	s_stat	file_stat;
	if ( stat( path.c_str(), &file_stat ) == -1 ) {
		this->setCodeAndMessageError("404", "Not Found: The requested resource could not be found");
		status = NOTFOUND;
		return ;
	}
	if ( S_ISDIR( file_stat.st_mode ) ) {
		DIR	*dir = opendir( path.c_str() );
		if ( !dir || access( path.c_str(), W_OK ) ) {
			this->setCodeAndMessageError("403", "Forbidden: Access to this resource is denied");
			status = FORBIDEN;
			return ;
		}
		struct dirent	*entry;
		while ( ( entry = readdir(dir) ) != NULL ) {
			const char	*name = entry->d_name;
			if ( !strcmp(name, ".") || !strcmp(name, "..") )
				continue ;
			std::string	child = path;
			if ( child.size() && child.at( child.size() - 1 ) != '/' )
				child += '/';
			child += name;
			this->setDelete( child, status );
			if ( status != SUCCES ) {
				closedir(dir);
				return ;
			}
		}
		closedir( dir );
		if (std::remove( path.c_str() ) ) {
			this->setCodeAndMessageError("500", "Internal server error");
			status = INTERNERROR;
			return ;
		}
		this->setCodeAndMessageError("200", "OK: The resource has been deleted");
		status = SUCCES;
		return ;
	} else {
		if ( access( path.c_str(), W_OK ) || access( path.c_str(), R_OK ) )  {
			this->setCodeAndMessageError("403", "Forbidden: Access to this resource is denied");
			status = FORBIDEN;
			return ;
		}
		if (std::remove( path.c_str() ) ) {
			this->setCodeAndMessageError("500", "Internal server error");
			status = INTERNERROR;
			return ;
		}
		this->setCodeAndMessageError("200", "OK: The resource has been deleted");
		status = SUCCES;
	}
	if ( S_ISDIR(file_stat.st_mode) ) {
		this->setCodeAndMessageError("403", "Forbidden: Access to this resource is denied");
		status = FORBIDEN;
		return ;
	}
}

ServerFD*	Request::getServer( void ) {
	return (this->_server_fd);
}

