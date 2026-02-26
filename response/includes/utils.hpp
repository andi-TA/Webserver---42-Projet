
#ifndef UTILS_HPP
# define UTILS_HPP

# include <iostream>
# include <string>
# include <fstream>
# include <fcntl.h>
# include <unistd.h>
# include <sstream>
# include <string>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/wait.h>
#include <filesystem>

# define SIZE_BUFF 4096

    int	        ftGetLastChar( std::string str );
    bool	    ftUriIsDir( std::string uri, std::string root );
    bool	    ftHttpIsPresent( std::string uri );
    void	    ftFreeTab( char ***tab );
    std::string ftGetExtensionInFile( std::string file );
    std::string	ftBuildHeadHtmlDirList( void );
    std::string	ftGetScriptFilename( std::string uri );
    std::string	ftGetContentTypeWithPost( std::string request );
    std::string	ftBuildHeadHtmlError( std::string error_code );
    std::string	ftGetSubContent( std::string & body );
    std::string	ftGetLastModifyFile( std::string file );
    std::string	ftGetSizeFile( std::string file );
    std::string	ftAddPointInPath( std::string path );
    std::string	ftGetDirInPath( std::string &path );
#endif