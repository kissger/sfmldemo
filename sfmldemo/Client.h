#ifndef CLIENT_H
#define CLIENT_H

#include <SFML/Network.hpp>
#include <SFML/Graphics.hpp>
#include <iostream>
#include "MessageObject.hpp"

class Client
{
public:
	Client(unsigned int, sf::IpAddress, std::string);
	~Client();
	bool canRemove;

	sf::TcpSocket* getSocket();
	void sendEventMessage(sf::Event);

private:
	sf::TcpSocket server;
	sf::IpAddress address;
	unsigned short port;
	bool isRunning;
	sf::Thread input;

	void send(std::string);
	void send(MessageObject);
	void send(unsigned short, std::string);

	MessageObject recieve();
	void manageClient();
	void getInput();
	void shutDown();
};

#endif //CLIENT_H