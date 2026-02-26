#ifndef ISERVER_HPP
#define ISERVER_HPP

#include <iostream>

class IServer
{
	public:
		virtual ~IServer() {};
		virtual std::string const&	getRequest() const = 0;
};

#endif