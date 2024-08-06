#include <vector>
#include <iostream>
#include <cstdlib>

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
		bool	get_channel_topic_status(){ return this->_topic_status; }
		std::string	get_channel_topic(){ return this->_topic; }

		// Setters //

		void	set_channel_name( std::string name ){ this->_name = name; }
		void	set_key_mode( bool status ) { _key_status = status; }
		void	set_invite_mode( bool status ) { _invite_status = status; }
		void	set_topic_mode( bool status ) { _topic_status = status; }
		void	set_limit_mode( bool status ) { _limit_status = status; }
		void	remove_operator_status( Client client );
		void	set_channel_password( std::string password ) { _password = password; }
		void	set_user_limit( unsigned long int limit ) { _user_limit = limit; }
		void	set_channel_topic( std::string new_topic ) { _topic = new_topic; }

		// Functions //

		void	add_new_client( Client client ){ this->_client_register.push_back(client); }
		void	add_operator( Client client ){ this->_operator_register.push_back(client); }
		bool	check_existing_client( int fd );
		bool	check_operator_status( int fd );
		void	send_message_to_client( std::string message, Client *client );
		void	send_message( int fd, std::string message ){ send(fd, message.c_str(), message.length(), 0); }

		// Clear Functions //

		void	client_clear( int fd );

	private :

		std::string	_name;
		std::string _topic;

		std::vector<Client> _client_register;
		std::vector<Client> _operator_register;

		bool	_invite_status;
		bool 	_topic_status;
		bool 	_key_status;
		bool	_limit_status;

		std::string _password;
		unsigned long int _user_limit;
};