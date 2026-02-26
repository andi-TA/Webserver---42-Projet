/*
	SessionManager.hpp
*/
#ifndef SESSIONMANAGER_HPP
# define SESSIONMANAGER_HPP

#include <map>
#include <ctime>
#include <string>
#include <cstdlib>
#include <sstream>
#include <unistd.h>

class SessionManager {
	private:
		SessionManager();
		~SessionManager();

	public:
		static std::string							createSession();
		static bool									hasSession(const std::string& id);
		static void									touchSession(const std::string& id);
		static std::map<std::string, std::string>&	getSessionData(const std::string& id);
};

#endif	// SessionManager.hpp
