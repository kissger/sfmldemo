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

std::string getNickname()
{
	std::string nick;
	std::cout << "enter a nickname\n";
	std::cin >> nick;
	return nick;
}

int main()
{
	
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