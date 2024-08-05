#include "Channel.hpp"

// FUNCTIONS //

bool	Channel::check_existing_client( int fd ) {

	for (size_t i = 0; i < _client_register.size(); i++) {

		if (_client_register[i].get_client_fd() == fd)
			return true;
	}
	return false;
}

bool	Channel::check_operator( int fd ) {

	for (size_t i = 0; i < _o_mode.size(); i++) {

		if (_o_mode[i].get_client_fd() == fd)
			return true;
	}
	return false;
}

void	Channel::send_message_to_client( std::string message, Client *client ) {

	for (size_t i = 0; i < _client_register.size(); i++) {

		if (_client_register[i].get_client_fd() != client->get_client_fd())
			send_message(_client_register[i].get_client_fd(), message);
	}
}

void	Channel::set_false_status(){
	_i_mode = false;
	_t_mode = false;
	_k_mode = false;
	_l_mode = false;
}

void	Channel::remove_o_mode( Client client ){
	
	for (unsigned long int i = 0; i < _o_mode.size(); i++){
		if (client.get_client_fd() == _o_mode[i].get_client_fd()){
			_o_mode.erase(_o_mode.begin()+i);
		}
	}
}


// CLEAR FUNCTIONS //

void	Channel::client_clear( int fd ) {

	for (size_t i = 0; i < _client_register.size(); i++) {

		if (_client_register[i].get_client_fd() == fd)
			{_client_register.erase(_client_register.begin() + i); break;}
	}
}