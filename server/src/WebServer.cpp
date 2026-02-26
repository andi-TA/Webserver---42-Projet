#include "../include/WebServer.hpp"

//  static attribut

std::vector< std::vector< std::string > >::iterator	WebServer::_it_vec;
std::vector< std::vector< std::string > >			WebServer::_vec_vec;
std::vector< std::string >::iterator				WebServer::_it_str;
std::vector< ServerFD* >::iterator					WebServer::_it_serv;
std::vector< std::string >							WebServer::_vec;
std::vector< ServerFD* >							WebServer::_server_config;
std::string											WebServer::_delim[3];
std::string											WebServer::_tmp;
std::vector< ServerFD* >							WebServer::_vec_serv;

PollFD*												WebServer::_poll_event;




/**
 * ******************** PUBLIC Methods ********************
 *  ->  WebServer::getAllServer( std::string const& file_name )
 *  ->  
 */

WebServer::~WebServer( void ) {

	//	Destructor where we free all AFD*
	WebServer::freeServer();
}

void	WebServer::getAllServer( std::string const& file_name, PollFD* poll_event ) {

	std::ifstream   									ifile(file_name.c_str());
	ServerFD											*server_tmp = NULL;
	
	WebServer::_tmp.clear();
	if (ifile.fail())
		throw std::invalid_argument("Arg_error: Can'open file \'" + file_name + "\'");
	std::getline(ifile, WebServer::_tmp, '\0');
	ifile.close();

	WebServer::_tmp = WebServer::cleanConfigContent(WebServer::_tmp);
	WebServer::_vec_vec = WebServer::getAllBlock(WebServer::_tmp);

	//	get all ServerFD
	WebServer::_it_vec = WebServer::_vec_vec.begin();
	while (WebServer::_it_vec != WebServer::_vec_vec.end() && (*WebServer::_it_vec).size()) {
		try{			
			if (!(*WebServer::_it_vec).size())
				break ;
			if ((*WebServer::_it_vec).at(0) != SERVER || directiveLen(*WebServer::_it_vec) != 1)
				throw std::invalid_argument("Content_error: on config file (Server Block)");
			WebServer::_it_vec++;
			server_tmp = new ServerFD();
			if (server_tmp == NULL)
				throw std::invalid_argument("Memory_alloc_error: allocation error (Server Block)");
			server_tmp->setServerBloc(WebServer::_it_vec);
		}
		catch(const std::exception& e) {
			WebServer::freeServer();
			if (server_tmp != NULL)
				delete (server_tmp);
			throw std::invalid_argument(e.what());
		}
		WebServer::_server_config.push_back(server_tmp);
		server_tmp = NULL;
	}
    WebServer::removeMultiPort();
	WebServer::splitServer();
	if (WebServer::getServerTab().size() == 0)
		throw   std::invalid_argument("Error: No server block found");
	WebServer::_poll_event = poll_event;
}

size_t	WebServer::directiveLen( std::vector<std::string>& content ) {
	
    //  get instruction word length
    size_t	len = 0;

	for ( std::vector<std::string>::iterator it = content.begin(); \
        it != content.end() && (*it) != ";"; it++)
		len++;
	return (len);
}

std::string&	WebServer::cleanConfigContent( std::string& content ) {

	//	Identify blocs and directives delimiters (';', '{', '}') and replace
	size_t		pos = 0;
	WebServer::_delim[0] = "{";
	WebServer::_delim[1] = "}";
	WebServer::_delim[2] = ";";
	
	for (size_t i = 0; i < 3; i++) {
		pos = content.find(WebServer::_delim[i], 0);
		while (pos != std::string::npos) {
			if (WebServer::existWord(content, pos + 1))
				throw std::invalid_argument("Content_error: on config file");
			content.replace(pos, 1, " " + WebServer::_delim[i] + " ");
			pos = content.find(WebServer::_delim[i], pos + 3);
		}
	}
	return (content);	
}

void	WebServer::removeMultiPort( void ) {

	WebServer::_vec.clear();
	for (size_t i = 0; i < WebServer::_server_config.size(); i++) {
		if (WebServer::_server_config[i]->getFDType() != SERVER)
			continue ;

		ServerFD*	server = dynamic_cast<ServerFD*>(WebServer::_server_config[i]);
	
		for (size_t j = 0; j < server->getMultiPort().size(); j++) {
			if (std::find(WebServer::_vec.begin(), WebServer::_vec.end(), \
				server->getMultiPort()[j]) != WebServer::_vec.end())
				throw	std::invalid_argument("same host:port (not allowed)");
			WebServer::_vec.push_back(server->getMultiPort()[j]);
		}		
	}
}

void	WebServer::splitServer( void ) {

	size_t		len = WebServer::_server_config.size();
	
	WebServer::_tmp.clear();

	for (size_t j = 0; j < len; j++) {
		if (WebServer::_server_config[j]->getFDType() != SERVER)
			continue ;

		ServerFD*	server = dynamic_cast<ServerFD*>(WebServer::_server_config[j]);
		
		WebServer::_tmp = server->getMultiPort()[0];

		server->setHost(WebServer::_tmp.substr(0, WebServer::_tmp.rfind(":")));
		server->setPort(WebServer::_tmp.substr(WebServer::_tmp.rfind(":") + 1));
		for (size_t i = 1; i < server->getMultiPort().size(); i++) {
			ServerFD	*tmp = new ServerFD(*server);

			WebServer::_tmp.clear();
			WebServer::_tmp = tmp->getMultiPort()[i];
			server->setHost(WebServer::_tmp.substr(0, WebServer::_tmp.rfind(":")));
			server->setPort(WebServer::_tmp.substr(WebServer::_tmp.rfind(":") + 1));
			WebServer::_server_config.push_back(tmp);
		}
	}
}

void	WebServer::launchServer() {
	struct addrinfo	hints;
	struct addrinfo	*res;

	for (std::vector<ServerFD *>::iterator it = WebServer::_server_config.begin(); \
		it != WebServer::_server_config.end(); it++) {
		
		if ((*it)->getFDType() != SERVER)
			continue ;
		memset(&hints, 0, sizeof(hints));
		hints.ai_family = AF_INET;
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_flags = AI_PASSIVE;


		int err = getaddrinfo((*it)->getHost().c_str(), (*it)->getPort().c_str(), &hints, &res);
		if (err != 0) {
			freeaddrinfo(res);
			throw std::domain_error(std::string("getaddrinfo error : ") + gai_strerror(err));
		}

		(*it)->setFD(socket(res->ai_family, res->ai_socktype, res->ai_protocol));
		if ((*it)->getFD() == -1) {
			freeaddrinfo(res);
			throw std::domain_error("Error : socket ()");
		}

		int opt = 1;
		if (setsockopt((*it)->getFD(), SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
			freeaddrinfo(res);
			throw std::domain_error("Error : setsockopt ()");
		}

		if (bind((*it)->getFD(), res->ai_addr, res->ai_addrlen) < 0) {
			freeaddrinfo(res);
			throw std::domain_error("Error : bind ()");
		}

		if (listen((*it)->getFD(), SOMAXCONN)) {
			freeaddrinfo(res);
			throw std::domain_error("Error : listen ()");
		}
		freeaddrinfo(res);
	}
}

std::vector<ServerFD*>&	WebServer::getServerTab( void ) {
	return (WebServer::_server_config);
}










/**
 * ******************** PRIVATE Methods ********************
 * 
 */

void	WebServer::freeServer( void ) {
	WebServer::_vec_serv.clear();
	WebServer::_vec_vec.clear();
	WebServer::_vec.clear();
	WebServer::_tmp.clear();
	if (!WebServer::_server_config.size())
		return ;
	for (std::vector<ServerFD*>::iterator it = WebServer::_server_config.begin(); \
        it != WebServer::_server_config.end(); it++) {
		delete	(*it);
		(*it) = NULL;
	}
	WebServer::_server_config.clear();
}

bool	WebServer::existWord( std::string const& str, size_t begin ) {
	
    //  To check if there is another instruction after ';' and return false if they is.
    for (size_t i = begin; i < str.length() && str[i] != '\n'; i++) {
		if (!std::isspace(str[i]))
			return (true);
	}
	return (false);
}

std::vector< std::vector< std::string > >	WebServer::getAllBlock( std::string& file_content ) {

    //  Store all of the config file instruction blocs in an array (vector)
    file_content = WebServer::cleanConfigContent(file_content);
	
    std::vector< std::vector< std::string > >	all_line;
	std::vector<std::string>					one_line;
	std::stringstream							ss(file_content);
	std::string									wrd;
	int											bracket_status = 0;

	if (ss.fail() || ss.eof())
		throw std::invalid_argument("Content_error: on config file");
	do
	{
		ss >> wrd;
		do
		{
			if (wrd.length() && wrd != "{" && wrd != "}" && !ss.eof() && wrd != ";") {
				one_line.push_back(wrd);
			}
			if (wrd == "{")
				bracket_status++;
			else if (wrd == "}") {
				if (all_line.size())
					all_line[all_line.size() - 1].push_back("}");
				bracket_status--;
			}
			if (bracket_status < 0)
				throw std::invalid_argument("Content_error: brackets don't match");
			wrd.clear();
			ss >> wrd;
		} while (!ss.eof() && wrd.length() && wrd != "{" && wrd != "}" && wrd != ";");
		if (one_line.size()) {
			if (wrd == ";")
				one_line.push_back(";");
			if (!ServerFD::isServerWordKey(one_line.at(0)) &&\
                !LocationConfig::isLocationWordKey(one_line.at(0)) && one_line.at(0) != "server") {
				std::string	error = "Content_error: unknown directive : \'" + one_line.at(0) + "\'";
				throw std::invalid_argument(error);
			}
			all_line.push_back(one_line);
		}
		if (wrd == "{")
			bracket_status++;
		else if (wrd == "}") {
			if (all_line.size())
				all_line[all_line.size() - 1].push_back("}");
			bracket_status--;
		}
		if (bracket_status < 0)
			throw std::invalid_argument("Content_error: brackets don't match");
		one_line.clear();
		wrd.clear();
	} while (!ss.eof());
	if (bracket_status != 0)
		throw std::invalid_argument("Content_error: not closed bracket");
	one_line.clear();
	all_line.push_back(one_line);
	return (all_line);
}

void	WebServer::display( int fd, std::string const& mssg, std::string const& color = RESET ) {
	if (WebServer::_poll_event->getFdRevent(fd) & POLLOUT)
		std::cout << color << mssg << BG_BLACK << BRIGHT_BLACK << RESET;
	else {
		if (WebServer::_poll_event->getFdRevent(fd) & POLLOUT)
			std::cerr << RED << mssg << BG_BLACK << BRIGHT_BLACK << RESET;
	}
}

void	WebServer::closeFD( int& fd ) {
	if (fd == -1)
		return ;
	close(fd);
}


void	WebServer::sliceSlash( std::string& path ) {
	for ( size_t i = 0; i < path.size(); i ++ ) {
		if (i < path.size() && path.at(i) == '/')
			i++;
		for ( ;i < path.size() && path.at(i) == '/'; )
			path.erase(i, 1);
	}
}
