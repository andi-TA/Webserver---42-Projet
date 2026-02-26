/*
	SessionManager.cpp
*/
#include "SessionManager.hpp"

static std::map<std::string, std::map<std::string, std::string> >	g_sessions;
static std::map<std::string, std::time_t>							g_sessions_last_access;

static std::string genRandomSessionId() {
	static bool seeded = false;
	if (!seeded) {
		std::srand((unsigned int)(std::time(NULL) ^ getpid()));
		seeded = true;
	}
	std::ostringstream oss;
	for ( int i = 0; i < 24; ++i ) {
		int r = std::rand() % 62;
		char c;
		if ( r < 10 ) {
			c = '0' + r;
		} else if ( r < 36 ) {
			c = 'A' + ( r - 10 );
		} else {
			c = 'a' + ( r - 36 );
		}
		oss << c;
	}
	return ( oss.str() );
}

std::string SessionManager::createSession() {
	std::string id;
	do {
		id = genRandomSessionId();
	} while ( g_sessions.find(id) != g_sessions.end() );
	g_sessions[id] = std::map<std::string, std::string>();
	g_sessions_last_access[id] = std::time(NULL);
	return ( id );
}

bool SessionManager::hasSession( const std::string& id ) {
	return ( g_sessions.find(id) != g_sessions.end() );
}

std::map<std::string, std::string>& SessionManager::getSessionData( const std::string& id ) {
	return ( g_sessions[id] );
}

void SessionManager::touchSession( const std::string& id ) {
	g_sessions_last_access[id] = std::time( NULL );
}

SessionManager::SessionManager() {}
SessionManager::~SessionManager() {}
