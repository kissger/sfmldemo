#include "Server.h"

/**
* constructor
* server starts listening on the specified port
* also starts the thread for user input and starts waiting for clients
*/
Server::Server(unsigned int port_) : port(port_)
{
	map = new Map(700, 700);
	std::cout << "Server started\nListening on port " << port << std::endl
			  << "public ip address: " << sf::IpAddress::getPublicAddress().toString() << std::endl
			  << "local ip address:  " << sf::IpAddress::getLocalAddress() << std::endl;
	listener.listen(port);
	selector.add(listener);
	isRunning = true;
	canConnect = true;
	map = new Map(700, 700);
	playa = new Player();
	launch();
}

void Server::launch()
{
	sf::Thread input(&Server::getInput, this);
	sf::Thread thread(&Server::waitForClients, this);
	thread.launch();
	input.launch();
}

void Server::manageClientMessages()
{
	sf::TcpSocket* toRemove = 0;
	ClientManager* managerToRemove = 0;
	//sf::Mutex mutex;
	//mutex.lock();
	for (std::list<ClientManager*>::iterator it = cms.begin(); it!=cms.end(); ++it)
	{
		ClientManager* cm = *it;
		sf::TcpSocket* client = cm->getSocket();
		if (selector.isReady(*client))
		{
			MessageObject msg;
			sf::Packet packet;
			sf::TcpSocket::Status status = client->receive(packet);
			packet >> msg;
						
			if (status == sf::Socket::Done) {
				//if (msg.type == MessageObject::GAMOV && msg.message == "gameover")
				//{

				//}
				if (msg.type == MessageObject::START && msg.message == "start")
				{
					std::cout << "no more player\n";
					canConnect = false;
					//MessageObject m(MessageObject::START, "start");
					//sendAll(m);
				}
				if (msg.type == MessageObject::NEWPL && msg.message == "newplayer")
				{
					recieveNewPlayer(client);
					//cm->appendMessage(msg);
				}
				if (msg.type == MessageObject::PLAYER && msg.message == "player")
				{
					sf::Mutex mutex;
					mutex.lock();
					sf::Packet packet;
					client->receive(packet);
					std::stringstream ss;
					ss.write((char*)packet.getData(), packet.getDataSize());
					Player* plocal = new Player();
					ss >> *plocal;
					//Player* player;// = getFromPacket<Player>(packet);
					//Player player;
					std::cout << "player update: " << plocal->getName() << std::endl;
					map->updatePlayer(plocal);
					updatePlayers(putToPacket(*plocal), client);
					mutex.unlock();
				}
				else
				{
					sf::Mutex mutex;
					mutex.lock();
					cm->appendMessage(msg);
					//sendAllExceptSender(msg, *client);
					sendAll(msg);
					mutex.unlock();
				}
			}
			else if (status == sf::Socket::Disconnected)
			{
				std::cout << "client has disconnected from " << client->getRemoteAddress() << std::endl;
				selector.remove(*client);
				toRemove = client;
				managerToRemove = cm;
			}
		}
	}
	//mutex.unlock();
	if (toRemove!=0)
	{
		sf::Mutex m;
		m.lock();
		delete toRemove;
		managerToRemove->shutDown();
		cms.remove(managerToRemove);
		delete managerToRemove;
		m.unlock();
	}
	if (cms.size() == 0)
	{
		restartAll();
	}
}

void Server::connectNewClient(sf::TcpSocket* client)
{
	MessageObject m = recieve(*client);
	std::cout << "client connected " << client->getRemoteAddress() << std::endl
				<< "mgr> " << m.message << std::endl;

	initGameProtocol(client);
	
	//starting client manager
	ClientManager* cm = new ClientManager(client, m.message);
	sendCurrentClients(*client);
	cms.push_back(cm);
	cm->run();
	selector.add(*client);
	sendAllExceptSender(m, client);

	//sending hi message
	send("I'm not the server You're looking for...", *client);
}

void Server::initGameProtocol(sf::TcpSocket* client)
{
	//sending map
	client->send(putToPacket(*map));
}

void Server::recieveNewPlayer(sf::TcpSocket* client)
{
	//recieving player
	sf::Packet packet;
	client->receive(packet);
	Player* player = getFromPacket<Player>(packet);
	std::cout << "player connected: " << player->getName() << std::endl;

	MessageObject m(MessageObject::NEWPL, "newplayer");
	sendAllExceptSender(m, client);
	//sendAll(m);
	sf::Packet playerpacket = putToPacket(*player);
	sendPacketAllExceptSender(playerpacket, client);
	//sendPacketAll(playerpacket);
	/*std::vector<Tank*> tanks = player->getTanks();
	for (int i = 0; tanks.size()<10 && i<tanks.size(); ++i)
	{
		std::printf("\t%d %d\n", tanks[i]->getPosX(), tanks[i]->getPosY());
	}*/

	map->add(player);
}

/**
* wating for clients, this method runs in its own thread
* if a client connects, sends a welcome message
* if a client sends a message, displays it
* if a client disconnects, remove it from the clients
*/
void Server::waitForClients()
{
	while(isRunning)
	{
		if (selector.wait())
		{
			if (selector.isReady(listener))
			{
				if (canConnect)
				{
					sf::TcpSocket* client = new sf::TcpSocket;
					if (listener.accept(*client) == sf::Socket::Done)
					{
						connectNewClient(client);
					}
				}
			}
			else
			{
				manageClientMessages();
			}
		}
	}
}

void Server::updatePlayers(sf::Packet& packet, sf::TcpSocket* client)
{
	MessageObject update(MessageObject::UPD, "update");
	sendAllExceptSender(update, client);
	//sendAllexc(update);
	sendPacketAllExceptSender(packet, client);
	//sendPacketAll(packet);
}

/**
* recieves MessageObject from the given client
* @param TcpSocket the client
* @return MessageObject
*/
MessageObject Server::recieve(sf::TcpSocket& client)
{
	sf::Packet packet;
	client.receive(packet);
	MessageObject m;
	packet >> m;
	return m;
}

/** TODO : remove this, we won't have commandline
* this method runs in its own thread
* gets input from commandline
*/
void Server::getInput()
{
	std::string in;
	while (isRunning)
	{
		getline(std::cin, in);
		if (in == "quit")
		{
			isRunning = false;
			shutDown();
		}
		else if (in == "unlock")
		{
			sf::Mutex m;
			m.lock();
			canConnect = true;
			m.unlock();
		}
		else if (in == "newgame")
		{
			canConnect = true;
			delete map;
			map = new Map(700, 700);
			MessageObject m(MessageObject::CMD, "shut");
			sendAll(m);
		}
		else if (in == "player")
		{
			MessageObject m(MessageObject::PLAYER, "player");
			sendAll(m);
			//sendPacketAll(p);
		}
		else
		{
			MessageObject m(in);
			sendAll(m);
		}
	}
}

void Server::sendPacketAll(sf::Packet& packet)
{
	sf::Mutex m;
	m.lock();
	for (std::list<ClientManager*>::iterator it = cms.begin(); it!=cms.end(); it++)
	{
		ClientManager* cm = *it;
		cm->getSocket()->send(packet);
	}
	m.unlock();
}

void Server::sendPacketAllExceptSender(sf::Packet& packet, sf::TcpSocket* client)
{
	sf::Mutex m;
	m.lock();
	for (std::list<ClientManager*>::iterator it = cms.begin(); it!=cms.end(); it++)
	{
		ClientManager* cm = *it;
		if (cm->getSocket()->getRemoteAddress()!=client->getRemoteAddress())
		{
			//std::cout << cm->getSocket()->getRemoteAddress() << " " << client->getRemoteAddress() << std::endl;
			cm->getSocket()->send(packet);
		}
	}
	m.unlock();
}

/**
* sends a message object to all of the clients, except to who sent it
* @param MessageObject the message
* @param TcpSocket the sender
*/
void Server::sendAllExceptSender(MessageObject m, sf::TcpSocket* sender)
{
	for (std::list<ClientManager*>::iterator it = cms.begin(); it!=cms.end(); ++it)
	{
		ClientManager* cm = *it;
		sf::TcpSocket* client = cm->getSocket();
		if (client->getRemoteAddress() != sender->getRemoteAddress())
		{
			//std::cout << client->getRemoteAddress() << " " << sender->getRemoteAddress() << std::endl;
			send(m, *client);
		}
	}
}

/**
* sends a message object to all of the clients
* @param MessageObject
*/
void Server::sendAll(MessageObject m)
{
	sf::Mutex mutex;
	mutex.lock();
	std::list<ClientManager*>::iterator it = cms.begin();
	while(it!=cms.end())
	{
		ClientManager* cm = *it;
		sf::TcpSocket* socket = cm->getSocket();
		send(m, *socket);
		//if (send(m, *socket) == sf::Socket::Done)
		//{
			++it;
		//}
		/*else
		{
			ClientManager* remove = cm;
			it = cms.erase(it);
			selector.remove(*socket);
			cm->shutDown();
			delete cm->getSocket();
			delete cm;
		}*/
	}
	mutex.unlock();
}

void Server::sendCurrentClients(sf::TcpSocket& target)
{
	for (std::list<ClientManager*>::iterator it = cms.begin(); it!=cms.end(); it++)
	{
		MessageObject m(MessageObject::ALLCT, (*it)->getNickname());
		send(m, target);
	}
}

/**
* sends a message object to the client
* @param MessageObject
* @param TcpSocket
*/
sf::Socket::Status Server::send(MessageObject m, sf::TcpSocket& client)
{
	sf::Packet packet;
	packet << m;
	return client.send(packet);
}

/**
* sends a general message object to the client
* @param string
* @param TcpSocket
*/
sf::Socket::Status Server::send(std::string message, sf::TcpSocket& client)
{
	MessageObject m(message);
	return send(m, client);
}

/**
* sends a message object to the client
* @param int the message type
* @param string the message
* @param TcpSocket
*/
sf::Socket::Status Server::send(unsigned short i, std::string message, sf::TcpSocket& client)
{
	MessageObject m(i, message);
	return send(m, client);
}

void Server::restartAll()
{
	std::cout << "server> everybody shot down, restarting...\n";
	canConnect = true;
	delete map;
	map = new Map(700, 700);
}

/**
* shuts down the server<br>
* sends a shutdown command message to all the clients, and closes the connection
*/
void Server::shutDown()
{
	MessageObject m(MessageObject::CMD, "shut");
	if (cms.size()!=0) {
		for (std::list<ClientManager*>::iterator it = cms.begin(); it!=cms.end(); ++it)
			(*it)->shutDown();
		sendAll(m);
	}
	listener.close();
}

/**
* destructor<br>
* if server was running, calls the shutdown method
*/
Server::~Server()
{
	if (isRunning)
		shutDown();
	delete map;
	delete playa;
	std::cout << "server has been closed\n";
}