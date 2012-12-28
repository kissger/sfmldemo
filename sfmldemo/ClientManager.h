#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include "Client.h"

class ClientManager
{
public:
	ClientManager(sf::TcpSocket*, std::string);
	~ClientManager();

	sf::TcpSocket* getSocket();
	std::string getNickname();
	bool running();
	void run();

private:
	//unsigned int port;
	//sf::IpAddress address;
	std::string nickname;
	sf::TcpSocket* client;
	bool isRunning;

	void process();
};