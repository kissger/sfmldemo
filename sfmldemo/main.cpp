#include <SFML/Graphics.hpp>

#include "Server.h"
#include "Client.h"

/**
* gets the port number from the console
* @return int
*/
int getport()
{
	int port;
	std::cout << "enter port number\nor enter 0 for default(54322)\n";
	std::cin  >> port;
	if (port==0) port = 54322;
	return port;
}

/**
* gets the ip address from the console
* @return IpAddress
*/
sf::IpAddress getip()
{
	sf::IpAddress addr;
	std::cout << "enter ip address in xxx.xxx.xxx.xxx form\n";
	std::cin >> addr;
	return addr;
}

/**
* gets the nickname from the console
* @return string
*/
std::string getNickname()
{
	std::string nick;
	std::cout << "enter a nickname\n";
	std::cin >> nick;
	return nick;
}

int main()
{
	/*
	std::vector<Block*> blocks;
	blocks.push_back(new Block(10, 10, 10, 10));

	sf::Packet packet = putPointerContainerToPacket(blocks);
	std::vector<Block*> result;
	getPointerContainerFromPacket<Block>(packet, result);
	std::cout << result[0]->getPosX() << " " << result[0]->getPosY() << std::endl;

	std::vector<Tank*> tanks;
	tanks.push_back(new Tank(78, 0, 0, 0, 0));
    tanks.push_back(new Tank(78, 0, 0, 0, 0));
    tanks.push_back(new Tank(78, 0, 0, 99, 0));

	packet = putPointerContainerToPacket(tanks);
	std::vector<Tank*> rtank;
	getPointerContainerFromPacket<Tank>(packet, rtank);
	std::cout << rtank[0]->getPosX() << " " << rtank[0]->getPosY() << std::endl;
	*/

	char c = ' ';
	std::cout << "Server - s, Client c\n";
	std::cin >> c;
	
	if (c=='s') {
		int port = getport();
		Server server(port);
	}
	else if (c=='c') {
		int port = getport();
		sf::IpAddress address = getip();
		std::string nickname = getNickname();
		Client client(port, address, nickname);
	}
	else
		std::cout << "I can break the rules, too. Good bye!\n";

	//system("pause");
	
	
    return 0;
}