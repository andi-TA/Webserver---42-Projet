#ifndef IPOLLFD_HPP
#define IPOLLFD_HPP

#include <iostream>
#include <ctime>
#include <vector>
// #include "AFD.hpp"

#define REQUEST_TIMEOUT		60 // en seconde
#define RESPONSE_TIMEOUT	60 // en seconde


# define OK 							"200"
# define NOT_FOUND 						"404"
# define FORBIDDEN 						"403"
# define BAD_REQUEST 					"400"
# define REQUEST_TIMEOUT_ERROR			"408"
# define METHOD_NOT_ALLOWED 			"405"
# define HTTP_VERSION_NOT_SUPPORTED 	"505"
# define NOT_IMPLEMENTED 				"501"
# define GATEWAY_TIMEOUT_ERROR			"504"
# define BAD_GATEWAY 					"502"
# define URI_TOO_LONG 					"414"
# define PAYLOAD_TOO_LARGE				"413"
# define INTERNAL_SERVER_ERROR			"500"
# define LOOP_REDIRECT					"508"

#define WRITE			1
#define READ			0

class ServerFD;

class IPollFD
{
	public:
		virtual ~IPollFD() {};
		virtual std::vector<struct pollfd>&	getAllPollFD( void ) = 0;
		virtual std::vector<ServerFD*>&		getAllServer( void ) = 0;
		virtual struct pollfd&				getPollFD( int i ) = 0;
		virtual ServerFD*					getServer( int fd ) = 0;
		virtual void						update( void ) = 0;
		virtual void						addPollFd( int fd, short event ) = 0;
		virtual void						setFdEvent( int fd, short event ) = 0;

		virtual short						getFdRevent( int fd ) = 0;
		virtual void						deletePollFD( int fd ) = 0;


};

#endif