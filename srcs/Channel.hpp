#include <vector>
#include <iostream>

#include <sys/types.h>
#include <sys/socket.h>

#include "Client.hpp"

class Channel {

	public :

		Channel(){};
		~Channel(){};

		// Getters //

		std::string	get_channel_name(){ return this->_name; }
		bool	get_channel_invite_status(){ return this->_invite_status; }

		// Setters //

		void	set_channel_name( std::string name ){ this->_name = name; }
		void	set_channel_invite_status( bool status ){ this->_invite_status = status; }
		void	set_false_status();
		void	set_password(std::string password) { _k_password = password; }
		void	set_limit(unsigned long int limit) { _limit_user = limit; }
		void	set_k_mode() { _k_mode = true; }
		void	set_i_mode() { _i_mode = true; }
		void	set_t_mode() { _t_mode = true; }
		void	set_l_mode() { _l_mode = true; }
		void	remove_k_mode() { _k_mode = false; }
		void	remove_i_mode() { _i_mode = false; }
		void	remove_t_mode() { _t_mode = false; }
		void	remove_l_mode() { _l_mode = false; }
		void	remove_o_mode( Client client );



		// Functions //

		void	add_new_client( Client client ){ this->_client_register.push_back(client); }
		void	add_operator( Client client ){ this->_o_mode.push_back(client); }

		bool	check_existing_client( int fd );
		bool	check_operator( int fd );
		void	send_message_to_client( std::string message, Client *client );
		void	send_message( int fd, std::string message ){ send(fd, message.c_str(), message.length(), 0); }

		// Clear Functions //

		void	client_clear( int fd );

	private :

	std::string	_name;
	std::string _topic;

	std::vector<Client> _client_register;
	bool _i_mode;
	bool _t_mode;
	bool _k_mode;
	std::string _k_password;
	bool _l_mode;
	unsigned long int _limit_user;
	std::vector<Client> _o_mode;

	bool	_invite_status;
};