#pragma once

#include <SFML/Network.hpp>
#include <SFML/Graphics.hpp>
#include <iostream>
#include <deque>
#include "MessageObject.h"

class Client
{
public:
	Client(unsigned int, sf::IpAddress, std::string);
	~Client();

	sf::TcpSocket* getSocket();
	bool isConnected();
	void shutDown();

	void send(std::string);
	MessageObject getLastMessage();
	void sendEventMessage(sf::Event&);

private:
	sf::TcpSocket server;
	sf::IpAddress address;
	unsigned int port;
	bool isRunning;
	sf::Socket::Status status;
	std::deque<MessageObject> messages;

	void send(MessageObject);
	void send(unsigned short, std::string);
	MessageObject recieve();

	void manageClient();
	void getInput();
	void launch();
};
