#include "Server.hpp"
#include "User.hpp"

Server::Server()
{
	serverSocket_ = -1;
	running_ = false;
	nfds_ = 1;
}

void Server::closeServerConnection()
{
	for (int i = 1; i < nfds_; i++)
		close(pollfds_[i].fd);
	if (serverSocket_ != -1)
		close (serverSocket_);
	std::cout << "Shutting down server..." << std::endl;
}

void Server::initServerConnection(int port)
{
	// Get protocol number for TCP
	struct protoent *proto = getprotobyname("tcp");
	if (!proto)
		throw std::runtime_error("Error: getprotobyname failed");
	// Creating the Server Socket
	serverSocket_ = socket(AF_INET, SOCK_STREAM, 0);
	if (serverSocket_ < 0)
		throw std::runtime_error("Error: cannot open socket");
	// Initialize pollfd for listening socket
	pollfds_[0].fd = serverSocket_;
	pollfds_[0].events = POLLIN;
	// Set SO_REUSEADDR to avoid "address already in use" error on restart
	int opt = 1;
	if (setsockopt(serverSocket_, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt)) < 0)
		throw std::runtime_error("Error: setsockopt failed");
	// Bind to all interfaces
	struct sockaddr_in addr;
	std::memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	if (bind(serverSocket_, (struct sockaddr *) &addr, sizeof(addr)) < 0)
		throw std::runtime_error("Error: bind failed");
	// Listen
	if (listen(serverSocket_, SOMAXCONN) < 0) 
		throw std::runtime_error("Error: listen failed");
	std::cout << "Server listening on port " << port << " ..." << std::endl;
}

void Server::run()
{
	running_ = true;
	while (running_)
	{
		int poll_count = poll(pollfds_, nfds_, 5000); // 5 sec timeout
		if (poll_count == 0)
			continue; // Timeout, no events, loop again
		if (poll_count < 0)
		{
			if (errno == EINTR)
				continue; // Interrupted by signal, just continue
			std::cerr << "Error: poll" << std::endl;
			break;
		}
		try
		{
			checkNewConnections(); // Check if listening socket is readable => new connection
		}
		catch(const std::exception& e)
		{
			std::cerr << e.what() << std::endl;
			continue;
		}
		listenClients();
	}
}

void Server::stop()
{
	running_ = false;
}

void Server::checkNewConnections()
{
	if (pollfds_[0].revents & POLLIN)
	{
		struct sockaddr_in clientAddr;
		socklen_t clientLen = sizeof(clientAddr);
		int clientSocket = accept(serverSocket_, (struct sockaddr*)&clientAddr, &clientLen);
		if(fcntl(clientSocket, F_SETFL, O_NONBLOCK) < 0)
			throw std::runtime_error("Error: fcntl failed, cannot connect client");
		if (clientSocket < 0)
			throw std::runtime_error("Error: accept failed");
		else
			addNewUser(clientSocket, clientAddr);
	}
}

void Server::addNewUser(int clientSocket, struct sockaddr_in clientAddr)
{
	if (nfds_ >= MAX_CLIENTS + 1)
	{
		std::cerr << "Max clients reached, rejecting connection" << std::endl;
		close(clientSocket);
	}
	else
	{
		User newUser(clientSocket, clientAddr);
		userList.push_back(newUser);
		pollfds_[nfds_].fd = clientSocket;
		pollfds_[nfds_].events = POLLIN;
		pollfds_[nfds_].revents = 0;
		nfds_++;

		std::cout << "Accepted connection from "
					<< inet_ntoa(clientAddr.sin_addr) << ":"
					<< ntohs(clientAddr.sin_port) << std::endl;
	}
}

void Server::listenClients()
{
	for (int i = nfds_ - 1; i >= 1; i--)
	{
		try
		{
			User user = findUserBySocket(pollfds_[i].fd);
			if (pollfds_[i].revents & POLLIN)
				ManageMsg(user);
		}
		catch(const NoSuchUser& e)
		{
			std::cerr << e.what() << '\n';
		}
		catch(const User::ClientDisconnected& e)
		{
			std::cout << "Client disconnected" << std::endl; //handle by someone esle
			close(pollfds_[i].fd);
			pollfds_[i] = pollfds_[nfds_ - 1];
			nfds_--;
		}
	}
}

void Server::ManageMsg(User user)
{
	try
	{
		std::string msg = user.receiveMsg();
		std::cout << msg;
		user.sendMsg("TA GUEULE !!!!\r\n");
	}
	catch(const User::ErrorReiceve& e)
	{
		std::cerr << e.what() << std::endl;
	}
}

const User& Server::findUserBySocket(int socket)
{
	std::vector<User>::iterator it;
	for (it = userList.begin(); it < userList.end(); it++)
	{
		if (it->getSocket() == socket)
			return (*it);
	}
	throw NoSuchUser();
}

const char* Server::NoSuchUser::what() const throw()
{
	return ("Error: user not found");
}