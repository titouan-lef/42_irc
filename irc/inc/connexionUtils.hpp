#pragma once

#include <iostream>
#include <netinet/in.h>

int parsePort(const std::string& port);
int createSocket();
void defineSocketOption(int serverSocket);
struct sockaddr_in createAddress(int port);