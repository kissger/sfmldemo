#ifndef CLIENT_H
#define CLIENT_H

#include <SFML/Network.hpp>
#include <SFML/Graphics.hpp>
#include <iostream>
#include <sstream>
#include "MessageObject.h"
//#include "Tank.h"

class Client
{
public:
	Client(unsigned int, sf::IpAddress, std::string);
	~Client();
	bool canRemove;

	sf::TcpSocket* getSocket();
	void sendEventMessage(sf::Event);
	bool isConnected();

private:
	sf::TcpSocket server;
	sf::IpAddress address;
	unsigned short port;
	bool isRunning;
	sf::Socket::Status status;

	void send(std::string);
	void send(MessageObject);
	void send(unsigned short, std::string);
	//void send(const Tank&);

	MessageObject recieve();
	void manageClient();
	void launch();
	void getInput();
	void shutDown();
};

#endif //CLIENT_H