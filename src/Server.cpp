
#include "Server.hpp"

Server::Server()
{
	serverSocket = -1;
	running = false;
	nfds = 1;
}

void	Server::closeServerConnection()
{
	for (int i = 1; i < nfds; i++)
		close(pollfds[i].fd);
	if (serverSocket != -1)
		close (serverSocket);
	std::cout << "Shutting down server..." << std::endl;
}

void	Server::initServerConnection(int port)
{
	// Get protocol number for TCP
	struct protoent *proto = getprotobyname("tcp");
	if (!proto)
		throw (std::runtime_error("Error: getprotobyname failed"));
	// Creating the Server Socket
	serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (serverSocket < 0) 
		throw (std::runtime_error("Error: cannot open socket"));
	// Initialize pollfd for listening socket
	pollfds[0].fd = serverSocket;
	pollfds[0].events = POLLIN;
	// Set SO_REUSEADDR to avoid "address already in use" error on restart
	int opt = 1;
	if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt)) < 0) 
		throw (std::runtime_error("Error: setsockopt failed"));
	// Bind to all interfaces
	struct sockaddr_in	addr;
	std::memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	if (bind(serverSocket, (struct sockaddr *) &addr, sizeof(addr)) < 0) 
		throw (std::runtime_error("Error: bind failed"));
	// Listen
	if (listen(serverSocket, SOMAXCONN) < 0) 
		throw (std::runtime_error("Error: listen failed"));
	std::cout << "Server listening on port " << port << " ..." << std::endl;
}

void	Server::run()
{
	running = true;
	while (running)
	{
		int poll_count = poll(pollfds, nfds, 5000); // 5 sec timeout
		if (poll_count < 0)
		{
			if (errno == EINTR)
				continue; // Interrupted by signal, just continue
			std::cerr << "Error: poll" << std::endl;
			break;
		}
		else if (poll_count == 0)
			continue; // Timeout, no events, loop again
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

void	Server::stop()
{
	running = false;
}

void	Server::checkNewConnections()
{
	if (pollfds[0].revents & POLLIN)
	{
		struct sockaddr_in client_addr;
		socklen_t client_len = sizeof(client_addr);
		int client_fd = accept(serverSocket, (struct sockaddr*)&client_addr, &client_len);
		if(fcntl(client_fd, F_SETFL, O_NONBLOCK) < 0)
			throw (std::runtime_error("Error: fcntl failed, cannot connect client"));
		if (client_fd < 0)
			throw (std::runtime_error("Error: accept failed"));
		else
		{
			if (nfds >= MAX_CLIENTS + 1)
			{
				std::cerr << "Max clients reached, rejecting connection" << std::endl;
				close(client_fd);
			}
			else
			{
				pollfds[nfds].fd = client_fd;
				pollfds[nfds].events = POLLIN;
				pollfds[nfds].revents = 0;
				nfds++;

				std::cout << "Accepted connection from "
							<< inet_ntoa(client_addr.sin_addr) << ":"
							<< ntohs(client_addr.sin_port) << std::endl;
			}
		}
	}
}

void	Server::listenClients()
{
	for (int i = 1; i < nfds; i++)
	{
		if (pollfds[i].revents & POLLIN)
		{
			char buffer[1024];
			int bytes_received = recv(pollfds[i].fd, buffer, sizeof(buffer) - 1, 0);
			if (bytes_received < 0)
				std::cerr << "recv error" << std::endl;
			else if (bytes_received == 0)	// Connection closed by client
			{
				std::cout << "Client disconnected" << std::endl;
				close(pollfds[i].fd);
				pollfds[i] = pollfds[nfds - 1];
				nfds--;
				i--;
			}
			else
			{
				buffer[bytes_received] = '\0';
				std::cout << buffer;
			}
		}
	}
}