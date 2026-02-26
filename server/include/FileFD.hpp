#ifndef FILEFD_HPP
# define FILEFD_HPP

#include "AFD.hpp"


/* ****************** HERIT FROM AFD CLASS ***************** */

class FileFD : public AFD {
	public:
		FileFD( int& fd );
		FileFD( int& fd, short event );
		FileFD( int& fd, std::string const& str, short event );
		FileFD( const FileFD & cpy );
		~FileFD( void );

		FileFD&	operator=( const FileFD & cpy );

		const std::string&	getFDType( void ) const;
		short				getEvents( void ) const;
		int&				getFD( void ) ;
		void				setFD( int fd );


		bool				shouldClose( void );
		void				setCloseYes( void );


		bool				writeContent( void );
		std::string&		getContent( void );

	private:
		FileFD( void );

		//	attributs
		std::string	_content;
		std::string	_tmp;
		// int			_fd_type;
		bool		_should_close;

};

#endif
