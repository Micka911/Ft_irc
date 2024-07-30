#include "Server.hpp"

void	Server::server_init( int port, std::string password ) {

	set_port(port);
	set_password(password);
	socket_creation();
	std::cout << "Connected and waiting for requests..." << std::endl;

	while (_signal == false)
	{
		if((poll(&_pollfd_register[0], _pollfd_register.size(), -1) < 0) && _signal == false)
			throw(std::runtime_error("poll() faild"));

		for (size_t i = 0; i < _pollfd_register.size(); i++)
		{
			if (_pollfd_register[i].revents & POLLIN)
			{
				if (_pollfd_register[i].fd == get_socket_fd())
					new_client_request();
				else
					data_receiver(_pollfd_register[i].fd);
			}
		}
	}
	close_socket_fd();
}

void	Server::socket_creation() {

	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = htons(get_port());

	// Creating socket file descriptor
	set_socket_fd(socket(AF_INET, SOCK_STREAM, 0));
	if (get_socket_fd() < 0) {
		throw(std::runtime_error("failed to create socket"));
	}

	// Setting socket
	int opt = 1;
	if (setsockopt(get_socket_fd(), SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
		throw(std::runtime_error("failed to socket opt"));
	}
	if (fcntl(get_socket_fd(), F_SETFL, O_NONBLOCK) < 0) {
		throw(std::runtime_error("failed to fcntl"));
	}
	if (bind(get_socket_fd(), (struct sockaddr *)&addr, sizeof(addr)) < 0) {
		throw(std::runtime_error("failed to bind"));
	}
	if (listen(get_socket_fd(), SOMAXCONN) < 0) {
		throw(std::runtime_error("failed to listen"));
	}

	struct pollfd pollfd;
	pollfd.fd = get_socket_fd();
	pollfd.events = POLLIN;
	pollfd.revents = 0;
	_pollfd_register.push_back(pollfd);
}

void Server::new_client_request() {

	Client client;
	struct sockaddr_in addr;
	struct pollfd pollfd;
	socklen_t addr_len = sizeof(addr);

	int connect_fd = accept(get_socket_fd(), (sockaddr *)&(addr), &addr_len);
	if (connect_fd == -1)
		{std::cout << "accept() failed" << std::endl; return;}

	if (fcntl(connect_fd, F_SETFL, O_NONBLOCK) == -1)
		{std::cout << "fcntl() failed" << std::endl; return;}

	pollfd.fd = connect_fd;
	pollfd.events = POLLIN;
	pollfd.revents = 0;

	client.set_client_fd(connect_fd);
	client.set_client_ip_addr(inet_ntoa((addr.sin_addr)));
	_client_register.push_back(client);
	_pollfd_register.push_back(pollfd);

	std::cout << "Client [" << connect_fd << "] Connected" << std::endl;
}

void Server::data_receiver( int fd ) {

	char buff[1024];
	memset(buff, 0, sizeof(buff));

	ssize_t bytes = recv(fd, buff, sizeof(buff) - 1 , 0);

	if (bytes <= 0) {

		std::cout << "Client [" << fd << "] Disconnected" << std::endl;
		client_clear(fd);
		close(fd);
	}

	else {

		buff[bytes - 1] = '\0';
		std::cout << "Client [" << fd << "] Data : " << buff << std::endl;
		// Separate command in "cmd" "parameters"
		std::string cmd;
		cmd.assign(buff);
		std::vector<std::string> parse_cmd;
		int pos = 0;
		int prev_pos = 0;
		while ((pos = cmd.find(' ', pos)) != static_cast<int>(std::string::npos)){
			if (pos - prev_pos != 0)
				parse_cmd.push_back(cmd.substr(prev_pos, pos - prev_pos));
			prev_pos = ++pos;
		}
		if (pos - prev_pos != 0 && cmd.substr(prev_pos, pos - prev_pos)[0])
			parse_cmd.push_back(cmd.substr(prev_pos, pos - prev_pos));
		// Parsing data here
		unsigned long int i = 0;
		std::string er_msg = "Wrong command here, try another...\n";
		while (i < _list_cmd.size() && strcmp(parse_cmd[0].c_str(), _list_cmd[i].c_str()))
			i++;
		// IRC Protocool
		switch (i){
			case 0:
				pass_command(fd, parse_cmd);
				return ;
			case 1:
				nick_command(fd, parse_cmd);
				return ;
			case 2:
				user_command(fd, parse_cmd);
				return ;
			case 3:
				join_command(fd, parse_cmd);
				return ;
			case 4:
				part_command(fd, parse_cmd);
				return ;
			case 5:
				topic_command(fd, parse_cmd);
				return ;
			case 6:
				invite_command(fd, parse_cmd);
				return ;
			case 7:
				kick_command(fd, parse_cmd);
				return ;
			case 8:
				quit_command(fd, parse_cmd);
				return ;
			case 9:
				mode_command(fd, parse_cmd);
				return ;
			case 10:
				privmsg_command(fd, parse_cmd);
				return ;
			default:
				send(fd, er_msg.c_str(), er_msg.size(), 0);
				return ;
		}
	}
}

void	Server::client_clear( int fd ) {

	for (size_t i = 0; i < _pollfd_register.size(); i++) {

		if (_pollfd_register[i].fd == fd)
			{_pollfd_register.erase(_pollfd_register.begin() + i); break;}
	}

	for (size_t i = 0; i < _client_register.size(); i++) {

		if (_client_register[i].get_client_fd() == fd)
			{_client_register.erase(_client_register.begin() + i); break;}
	}
}

void	Server::close_socket_fd() {

	for(size_t i = 0; i < _client_register.size(); i++) {

		std::cout << "Client [" << _client_register[i].get_client_fd() << "] Disconnected" << std::endl;
		close(_client_register[i].get_client_fd());
	}

	if (get_socket_fd() != -1) {

		std::cout << "Server [" << get_socket_fd() << "] Disconnected" << std::endl;
		close(_socket_fd);
	}
}

bool Server::_signal = false;

void Server::signal_handler( int signum ) {

	(void)signum;
	std::cout << std::endl << "Signal Received !" << std::endl;
	_signal = true;
}

void Server::set_list_command(void) {

	_list_cmd.push_back("PASS");
	_list_cmd.push_back("NICK");
	_list_cmd.push_back("USER");
	_list_cmd.push_back("JOIN");
	_list_cmd.push_back("PART");
	_list_cmd.push_back("TOPIC");
	_list_cmd.push_back("INVITE");
	_list_cmd.push_back("KICK");
	_list_cmd.push_back("QUIT");
	_list_cmd.push_back("MODE");
	_list_cmd.push_back("PRIVMSG");
}

void	Server::pass_command(int fd, std::vector<std::string> parse_cmd){
	if (parse_cmd.size() == 2){
		send(fd, "<Password> = ", 13, 0);
		send(fd, parse_cmd[1].c_str(), parse_cmd[1].size(), 0);
		send(fd, "\n", 1, 0);
	}
	else {
		send(fd, "Bad parameters...", 17, 0);
		send(fd, "\n", 1, 0);
	}
}

void	Server::user_command(int fd, std::vector<std::string> parse_cmd){
	if (parse_cmd.size() == 5){
		send(fd, "<Username> = ", 13, 0);
		send(fd, parse_cmd[1].c_str(), parse_cmd[1].size(), 0);
		send(fd, "\n", 1, 0);
		send(fd, "<Hostname> = ", 13, 0);
		send(fd, parse_cmd[2].c_str(), parse_cmd[2].size(), 0);
		send(fd, "\n", 1, 0);
		send(fd, "<Servername> = ", 15, 0);
		send(fd, parse_cmd[3].c_str(), parse_cmd[3].size(), 0);
		send(fd, "\n", 1, 0);
		send(fd, "<Realname> = ", 13, 0);
		send(fd, parse_cmd[4].c_str(), parse_cmd[4].size(), 0);
		send(fd, "\n", 1, 0);
	}
	else {
		send(fd, "Bad parameters...", 17, 0);
		send(fd, "\n", 1, 0);
	}
}

void	Server::nick_command(int fd, std::vector<std::string> parse_cmd){
	if (parse_cmd.size() == 2){
		send(fd, "<Nickname> = ", 13, 0);
		send(fd, parse_cmd[1].c_str(), parse_cmd[1].size(), 0);
		send(fd, "\n", 1, 0);
	}
	else { 
		send(fd, "Bad parameters...", 17, 0);
		send(fd, "\n", 1, 0);
	}
}

void	Server::join_command(int fd, std::vector<std::string> parse_cmd){
	if (parse_cmd.size() == 2){
		send(fd, "<Channel> = ", 12, 0);
		send(fd, parse_cmd[1].c_str(), parse_cmd[1].size(), 0);
		send(fd, "\n", 1, 0);
	}
	else {
		send(fd, "Bad parameters...", 17, 0);
		send(fd, "\n", 1, 0);
	}
}

void	Server::part_command(int fd, std::vector<std::string> parse_cmd){
	if (parse_cmd.size() == 2){
		send(fd, "<Channel> = ", 12, 0);
		send(fd, parse_cmd[1].c_str(), parse_cmd[1].size(), 0);
		send(fd, "\n", 1, 0);
	}
	else {
		send(fd, "Bad parameters...", 17, 0);
		send(fd, "\n", 1, 0);
	}
}

void	Server::topic_command(int fd, std::vector<std::string> parse_cmd){
	if (parse_cmd.size() == 2){
		send(fd, "<Channel> = ", 12, 0);
		send(fd, parse_cmd[1].c_str(), parse_cmd[1].size(), 0);
		send(fd, "\n", 1, 0);
	}
	else {
		send(fd, "Bad parameters...", 17, 0);
		send(fd, "\n", 1, 0);
	}
}

void	Server::invite_command(int fd, std::vector<std::string> parse_cmd){
	if (parse_cmd.size() == 3){
		send(fd, "<Nickname> = ", 13, 0);
		send(fd, parse_cmd[1].c_str(), parse_cmd[1].size(), 0);
		send(fd, "\n", 1, 0);
		send(fd, "<Channel> = ", 12, 0);
		send(fd, parse_cmd[2].c_str(), parse_cmd[2].size(), 0);
		send(fd, "\n", 1, 0);
	}
	else {
		send(fd, "Bad parameters...", 17, 0);
		send(fd, "\n", 1, 0);
	}
}

void	Server::kick_command(int fd, std::vector<std::string> parse_cmd){
	if (parse_cmd.size() == 3){
		send(fd, "<Channel> = ", 12, 0);
		send(fd, parse_cmd[1].c_str(), parse_cmd[1].size(), 0);
		send(fd, "\n", 1, 0);
		send(fd, "<User> = ", 9, 0);
		send(fd, parse_cmd[2].c_str(), parse_cmd[2].size(), 0);
		send(fd, "\n", 1, 0);
	}
	else {
		send(fd, "Bad parameters...", 17, 0);
		send(fd, "\n", 1, 0);
	}
}

void	Server::quit_command(int fd, std::vector<std::string> parse_cmd){
	if (parse_cmd.size() == 1){
		send(fd, "You exited the server.", 22, 0);
		send(fd, "\n", 1, 0);
	}
	else {
		send(fd, "Bad parameters...", 17, 0);
		send(fd, "\n", 1, 0);
	}
}

void	Server::mode_command(int fd, std::vector<std::string> parse_cmd){
	if (parse_cmd.size() == 4){
		send(fd, "<Channel> = ", 12, 0);
		send(fd, parse_cmd[1].c_str(), parse_cmd[1].size(), 0);
		send(fd, "\n", 1, 0);
		send(fd, "<Mode [+ -]> = ", 15, 0);
		send(fd, parse_cmd[2].c_str(), parse_cmd[2].size(), 0);
		send(fd, "\n", 1, 0);
		send(fd, "<User> = ", 9, 0);
		send(fd, parse_cmd[3].c_str(), parse_cmd[3].size(), 0);
		send(fd, "\n", 1, 0);
	}
	else {
		send(fd, "Bad parameters...", 17, 0);
		send(fd, "\n", 1, 0);
	}
}

void	Server::privmsg_command(int fd, std::vector<std::string> parse_cmd){
	if (parse_cmd.size() >= 3){
		send(fd, "<Receiver> = ", 13, 0);
		send(fd, parse_cmd[1].c_str(), parse_cmd[1].size(), 0);
		send(fd, "\n", 1, 0);
		send(fd, "<Message> = ", 12, 0);
		for (unsigned long int i = 2; i < parse_cmd.size(); i++){
			send(fd, parse_cmd[i].c_str(), parse_cmd[i].size(), 0);
			send(fd, " ", 1, 0);
		}
		send(fd, "\n", 1, 0);
	}
	else {
		send(fd, "Bad parameters...", 17, 0);
		send(fd, "\n", 1, 0);	
	}
}