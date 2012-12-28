#include "ClientManager.h"

ClientManager::ClientManager(sf::TcpSocket* client_, std::string nick) :
	 nickname(nick), client(client_), isRunning(true)
{
	//run();
}

ClientManager::~ClientManager()
{
}

sf::TcpSocket* ClientManager::getSocket()
{
	return client;
}

std::string ClientManager::getNickname()
{
	return nickname;
}

void ClientManager::run()
{
	sf::Thread process(&ClientManager::process, this);
	process.launch();
}

bool ClientManager::running()
{
	return isRunning;
}

void ClientManager::process()
{
	while(isRunning)
	{
		sf::sleep(sf::milliseconds(10));
		MessageObject msg;
		sf::Packet packet;
		sf::TcpSocket::Status status = client->receive(packet);
		packet >> msg;
						
		if (status == sf::Socket::Done) {
			if (msg.message!="tank") {
				std::cout << "mgr " << nickname << "> " << msg << std::endl;
				//sendAllExceptSender(msg, client);
			}
			else
			{
				//recieveTank(client);
			}
		}
		else
		{
			isRunning = false;
		}
		std::cout << "loop\n";
	}
}