#include <cstring>
#include <signal.h>
#include "server/include/WebServer.hpp"
#include "server/include/ServerFD.hpp"
#include "server/include/PollFD.hpp"
#include "server/include/ClientFD.hpp"
#include "Request.hpp"
#include "Response.hpp"

PollFD	poll_event;

void	signal_handler(int signum) {
	if (signum == SIGINT) {
		WebServer::freeServer();
		exit(EXIT_SUCCESS);
	}
}



int main(int argc, char const *argv[])
{
	signal(SIGINT, signal_handler);
	try {
		if (argc > 2)
			throw   std::invalid_argument("Error: Only one file is required.");
		else if (argc == 2)
			poll_event.setConfigFile(argv[1]);
		else
			poll_event.setConfigFile("./config/default.conf");
	
		WebServer::getAllServer(poll_event.getConfigFile(), &poll_event);
		WebServer::launchServer();

		poll_event = PollFD(WebServer::getServerTab());

		poll_event.update();

		while (true)
		{
			int res = poll(poll_event.getAllPollFD().data(), \
				poll_event.getAllPollFD().size(), -1);
			if (res < 0) {
				throw std::invalid_argument("Error : poll() value ----> \'-1\'");
			}
			if (!res)
				continue;

			for (std::vector<ServerFD*>::iterator server = poll_event.getAllServer().begin(); \
				server != poll_event.getAllServer().end(); server++) {
				if (poll_event.getFdRevent((*server)->getFD()) & POLLIN) {
					(*server)->handleEvent(poll_event);
				}
				else {
					size_t	len = (*server)->getAllClient().size();

					if (len <= 0)
						continue ;

					std::map<int, ClientFD*>::iterator it = (*server)->getAllClient().begin();
					for (size_t i = 0; i < len; i++) {
						ClientFD*	client = (*it).second;
							
						client->handleEvent(poll_event);

						it++;

					}

					for (std::map<int, ClientFD*>::iterator it = (*server)->getAllClient().begin() ;
						it != (*server)->getAllClient().end();) {
						if ((*it).second->shouldClose()) {
							(*it).second->deleteFilesResponse(poll_event);
							poll_event.deletePollFD((*it).first);
							delete ((*it).second);
							(*server)->getAllClient().erase(it);
							if (!(*server)->getAllClient().size())
								break ;
							it = (*server)->getAllClient().begin();
						}
						else {
							++it;
						}
					}
				}
			}
		}
	}
	catch(const std::exception& e) {
		std::cerr << e.what() << '\n';
		WebServer::freeServer();
		return (0);
	}
	return 0;
}
