#include <unistd.h>
#include <string.h>

#include <vector>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <poll.h>

#include "Client.hpp"

class Server {

	public :

		Server(){ set_list_command(); };
		~Server(){};

		// Getters //
		int				get_socket_fd(){ return this->_socket_fd; };
		int				get_port(){ return this->_port; };
		std::string		get_password(){ return this->_password; };

		// Setters //
		void			set_socket_fd( int fd ){ this->_socket_fd = fd; };
		void			set_port( int port ){ this->_port = port; };
		void			set_password( std::string password ){ this->_password = password; };

		// Fonctions //
		void			server_init( int port, std::string password );
		void			socket_creation();
		void			new_client_request();
		void			data_receiver( int fd );

		void			client_clear( int fd );
		void			close_socket_fd();
		static void		signal_handler( int signum );

		// Command Functions //
		void			pass_command(int fd, std::vector<std::string> parse_cmd);
		void			user_command(int fd, std::vector<std::string> parse_cmd);
		void			nick_command(int fd, std::vector<std::string> parse_cmd);
		void			join_command(int fd, std::vector<std::string> parse_cmd);
		void			part_command(int fd, std::vector<std::string> parse_cmd);
		void			topic_command(int fd, std::vector<std::string> parse_cmd);
		void			invite_command(int fd, std::vector<std::string> parse_cmd);
		void			kick_command(int fd, std::vector<std::string> parse_cmd);
		void			quit_command(int fd, std::vector<std::string> parse_cmd);
		void			mode_command(int fd, std::vector<std::string> parse_cmd);
		void			privmsg_command(int fd, std::vector<std::string> parse_cmd);

	private :

	// Values //
		int							_port;
		int							_socket_fd;
		std::string					_password;
		std::vector<Client>			_client_register;
		std::vector<struct pollfd>	_pollfd_register;
		std::vector<std::string>	_cmd;
		std::vector<std::string>	_list_cmd;
		static bool					_signal;
	
	// Functions //
		void		set_list_command(void);
};