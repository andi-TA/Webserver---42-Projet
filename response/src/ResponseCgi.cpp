#include "Response.hpp"

void	Response::setFdPipeWithCgi( void ) {
	int			pipe_fd[2];
	int			pipe_fd_in[2];

	if (pipe(pipe_fd) == -1 || pipe(pipe_fd_in) == -1) 
		throw std::logic_error(INTERNAL_SERVER_ERROR);

	// POLL
	FileFD*	file1 = new FileFD(pipe_fd[0], POLLIN);
	FileFD*	file2 = new FileFD(pipe_fd[1], POLLOUT);
	FileFD*	file3 = new FileFD(pipe_fd_in[0], POLLIN);
	FileFD*	file4 = new FileFD(pipe_fd_in[1], POLLOUT);

	this->_fd_files.push_back(file1);
	this->_fd_files.push_back(file2);
	this->_fd_files.push_back(file3);
	this->_fd_files.push_back(file4);
}

bool	Response::writeInPipe( void )
{
	if (!this->_method.compare("POST"))
	{
		if (!this->_get_body)
		{
			this->_body = this->_request->getBody();
			this->_get_body = true;
		}
		if (this->_body.size() >= BUFSIZZ) {
			this->_tmp  = this->_body.substr(0, BUFSIZZ);
			this->_body.erase(0, this->_tmp.size());
		}
		else {
			this->_tmp = this->_body;
			this->_body.clear();
		}
		if (write(this->_fd_files[3]->getFD() , this->_tmp.c_str(), this->_tmp.size()) == -1) {
			this->_tmp.clear();
			this->_body.clear();
			throw std::logic_error(INTERNAL_SERVER_ERROR);
		}
		if (!this->_body.empty())
			return (false);
		return (true);
	}
	return (true);
}

bool	Response::readInPipe( void )
{
	int		n = 0;
	char	buff[4096];

	n = read(this->_fd_files[0]->getFD(), buff, sizeof(buff));
	if (n > 0)
	{
		this->_response_body.append(buff, n);
		return (false);
	}
	else if (n == -1)
		throw std::logic_error("READ ERROR (-1)");
	return (true);
}

bool	Response::sendResponseWithCgi( IPollFD& poll_event )
{
	if ( this->_cgi_step_stat == CGI_STEP_ONE ) {
		this->_pid = fork();
		if (this->_pid == -1)
			throw std::logic_error("Error: on proc fork");
		else if (this->_pid == 0)
		{
			if (chdir(ftAddPointInPath(ftGetDirInPath(this->_path)).c_str()) == -1)
			{
				ftFreeTab(&(this->_envp));
				exit(EXIT_FAILURE);
			}

			this->_envp = buildEnvp();

            WebServer::closeFD(this->_fd_files[0]->getFD());
            WebServer::closeFD(this->_fd_files[3]->getFD());

            dup2(this->_fd_files[1]->getFD(), STDOUT_FILENO);
            dup2(this->_fd_files[2]->getFD(), STDIN_FILENO);

            std::string scriptFilename = ftGetScriptFilename(this->_path);
            char	*argv[] = {
                const_cast<char *>(scriptFilename.c_str()),
                NULL
            };
            execve(this->_cgi.c_str(), argv, this->_envp);
            ftFreeTab(&(this->_envp));
            exit(EXIT_FAILURE);
		}
		else 
			this->_cgi_step_stat = CGI_STEP_TWO;
	}
	if (this->_cgi_step_stat == CGI_STEP_TWO) {
		int status = 0;
		int res = waitpid(this->_pid, &status, WNOHANG);
		short	revents = 0;
		switch (res) {
			case -1:
				throw std::logic_error(INTERNAL_SERVER_ERROR);
			case 0:
				revents = poll_event.getFdRevent(this->_fd_client);
				if ((revents & POLLHUP) || (revents & POLLERR) || (revents & POLLNVAL)) {

					this->killPid();
					waitpid(this->_pid, &status, WNOHANG);
					poll_event.deletePollFD(this->_fd_files[1]->getFD());
					poll_event.deletePollFD(this->_fd_files[2]->getFD());
					WebServer::closeFD(this->_fd_files[1]->getFD());
					WebServer::closeFD(this->_fd_files[2]->getFD());
					throw std::logic_error("UNAVAILABLE CLIENT\n");
				}

				revents = poll_event.getFdRevent(this->_fd_files[0]->getFD());
				if ((revents & POLLHUP) || (revents & POLLERR) || (revents & POLLNVAL)) {
					throw std::logic_error(INTERNAL_SERVER_ERROR);
				}
				if (revents & POLLIN) {
					this->readInPipe();
				}
				revents = poll_event.getFdRevent(this->_fd_files[3]->getFD());
				if ((revents & POLLHUP) || (revents & POLLERR) || (revents & POLLNVAL)) {
					throw std::logic_error(INTERNAL_SERVER_ERROR);
				}
				if (revents & POLLOUT) {
					this->writeInPipe();
				}
				break ;
					
			default:
				poll_event.deletePollFD(this->_fd_files[0]->getFD());
				poll_event.deletePollFD(this->_fd_files[1]->getFD());
				poll_event.deletePollFD(this->_fd_files[2]->getFD());
				poll_event.deletePollFD(this->_fd_files[3]->getFD());
				WebServer::closeFD(this->_fd_files[0]->getFD());
				WebServer::closeFD(this->_fd_files[1]->getFD());				
				WebServer::closeFD(this->_fd_files[2]->getFD());
				WebServer::closeFD(this->_fd_files[3]->getFD());
				this->_pid = -1;
				if (WIFEXITED(status)) {
					if (WEXITSTATUS(status) == EXIT_FAILURE) {
						throw std::logic_error(INTERNAL_SERVER_ERROR);
					}
				}
				this->_cgi_step_stat = CGI_STEP_THREE;
				break;
		}
	}
	if (this->_cgi_step_stat == CGI_STEP_THREE) 
	{
		if (!this->_cgi.empty()	&& 
			!ftGetExtensionInFile(ftGetScriptFilename(this->_path)).compare(".py"))
		{
			std::size_t	found = this->_response_body.find(CRLF_END_PYTHON);
			if (found != std::string::npos)
				this->_response_body.erase(0, found + strlen(CRLF_END_PYTHON));
		}
		this->_cgi_step_stat = CGI_STEP_FOUR;
	}
	if (this->_cgi_step_stat == CGI_STEP_FOUR)
	{
		std::size_t	found = this->_response_body.find(CRLF_END);
		if (found != std::string::npos)
		{
			std::string			header_block = this->_response_body.substr(0, found);
			std::istringstream	hstream(header_block);
			std::string			line;
			while ( std::getline( hstream, line ) ) {
				if (!line.empty() && line[line.size() - 1] == '\r') {
					line.erase(line.size() - 1, 1);
				}
				std::size_t		col = line.find(':');
				if (col == std::string::npos) {
					continue;
				}
				std::string		hname = line.substr( 0, col );
				std::string		hval = line.substr( col + 1 );
				hname.erase( 0, hname.find_first_not_of(" \t") );
				hname.erase( hname.find_last_not_of(" \t") + 1 );
				hval.erase( 0, hval.find_first_not_of(" \t") );
				hval.erase( hval.find_last_not_of(" \t") + 1 );
				if ( !hname.compare( "Set-Cookie" ) ) {
					this->addSetCookie( hval );
				}
			}
			this->_response_body.erase(0, found + strlen(CRLF_END));
		}
		this->_cgi_step_stat = CGI_STEP_FIVE;
	}
	if (this->_cgi_step_stat == CGI_STEP_FIVE) 
	{
		short	revents = poll_event.getFdRevent(this->_fd_client); 
		if ((revents & POLLHUP) || (revents & POLLERR) || (revents & POLLNVAL))
			throw std::logic_error(INTERNAL_SERVER_ERROR);
		if (!(revents & POLLOUT)) {
			poll_event.setFdEvent(this->_fd_client, POLLOUT);
			return (false);
		}
		this->sendHeader(this->_response_body);
		this->_cgi_step_stat = CGI_STEP_SIX;
	}
	if (this->_cgi_step_stat == CGI_STEP_SIX)
	{
		short	revents = poll_event.getFdRevent(this->_fd_client); 
		if ((revents & POLLHUP) || (revents & POLLERR) || (revents & POLLNVAL))
			throw std::logic_error(INTERNAL_SERVER_ERROR);
		if (!(revents & POLLOUT)) {
			poll_event.setFdEvent(this->_fd_client, POLLOUT);
			return (false);
		}
		if (this->_response_body.empty())
			return (true);
		this->_tmp = this->_response_body.substr(0, BUFSIZ);
		this->sendBody(this->_tmp);
		this->_response_body.erase(0, this->_tmp.size());
		if (this->_response_body.empty())
			return (true);
		return (false);
	}
	return (false);
}

bool	Response::rootRequestMatchWithConfigRoot( void )
{
	size_t path_size = this->_root.size();
	std::string	root_path = "";
	if (this->_path.size() > path_size)
		root_path = this->_path.substr(0, path_size);
	if (!root_path.empty() && !root_path.compare(this->_root))
		return (true);
	return (false);
}

bool	Response::checkRequestIfGood( void )
{
	if (this->_request_have_error)
	{
		this->_error_code = this->_request_error_code;
		this->_status = this->_request_message_error;
		if (!this->_error_code.compare("405") && !this->_redirect_uri.empty())
			return (true);
		this->buildErrorPage(this->_status, this->_error_code, this->_request_message_error);
		return (false);
	}
	return (true);
}
