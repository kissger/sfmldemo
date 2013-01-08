#include "Client.h"

/**
* constructs a client with the given port and ip address, 
* sets the default variables, and starts the input thread, 
* and also the manager thread for the client
* @param unsigned int port
* @param IpAddress IP address
*/
Client::Client(unsigned int port_, sf::IpAddress addr_, std::string nickname_) : port(port_), address(addr_),
	isRunning(true), nickname(nickname_)
{
	std::cout << "connecting...\n";
	status = server.connect(address, port);
	if (status == sf::Socket::Done) {
		std::cout << "Connection established\n";
		MessageObject hi(MessageObject::CONN, nickname);
		send(hi);
		
		initGameProtocol();

		launch();
	}
	else
		std::cout << "Connection not established\n";
}

void Client::initGameProtocol()
{
	//recieving map
	sf::Packet packet;
	server.receive(packet);
	map = getFromPacket<Map>(packet);

	//create and send player
	player = new Player(nickname);
	server.send(putToPacket(*player));
	map->add(player);
}

/**
* launches the clients threads, one for managig communication, 
* one for getting input from the console
*/
void Client::launch()
{
	sf::Thread manager(&Client::manageClient, this);
	sf::Thread input(&Client::getInput, this);
	manager.launch();
	input.launch();
}

/**
* @return bool true, if the connection to the server was successful
*/
bool Client::isConnected()
{
	return (status == sf::Socket::Done);
}

/**
* @return TcpSocket pointer to the client's socket
*/
sf::TcpSocket* Client::getSocket()
{
	return &server;
}

void Client::sendEventMessage(sf::Event& ev)
{
	std::stringstream ss;
	if (ev.type == sf::Event::KeyPressed && ev.key.code == sf::Keyboard::Home)
	{
		MessageObject m(MessageObject::START, "start");
		send(m);
	}
	if (ev.type == ev.MouseButtonReleased)
	{
		ss << ev.mouseButton.x << " " << ev.mouseButton.y;
		if (ev.mouseButton.button == sf::Mouse::Right)
		{
			send(MessageObject::ACTION, "user shot/moved to " + ss.str());
		}
		else if (ev.mouseButton.button == sf::Mouse::Left)
		{
			send(MessageObject::MVMNT, "user clicked at: " + ss.str());
		}
	}
}

/**
* manages the client, recieves and displays the messages from the server
*/
void Client::manageClient()
{
	while(isRunning)
	{
		sf::sleep(sf::milliseconds(10));
		MessageObject m = recieve();
		messages.push_back(m);
		std::cout << m << std::endl;
		if (m.type == MessageObject::UPD && m.message == "update")
		{

		}
		else if (m.type == MessageObject::CMD && m.message == "shut")
		{
			isRunning = false;
			shutDown();
		}
		else if (m.type==MessageObject::CMD && m.message == "tank")
		{			
			/*MessageObject m = recieve();
			int n = strtoint(m.message);
			for (int i = 0; i<n; ++i)
			{
				sf::Packet packet;
				server.receive(packet);
				Tank tank = getFromPacket<Tank>(packet);
				std::cout << tank.getPosX() << " " << tank.getPosY() << " " << tank.getSizeX() << " " << tank.getSizeY() << " " << tank.getTypeID() << std::endl;
			}*/
		}
	}
}

/**
* gets input from console, and sends it to the server
* this method runs in its own thread
*/
void Client::getInput()
{
	while (isRunning)
	{
		std::string in;
		getline(std::cin, in);
		if (in=="quit")
			shutDown();
		//else if (in=="tank")
		//{
			//MessageObject tankmsg(100, "tank");
			//send(tankmsg);
			//Tank t(11, 12, 13, 14, 15);
			//send(t);
		//}
		else if (in!="")
		{
			MessageObject m(100, in);
			send(m);
		}
	}
}

/**
* sends a general message
* @param string
*/
void Client::send(std::string message)
{
	MessageObject m(100, message);
	send(m);
}

/**
* sends a specific message
* @param MessageObject
*/

void Client::send(MessageObject message)
{
	sf::Packet packet;
	packet << message;
	server.send(packet);
}

/**
* sends a message constructed by the given type and message string
* @param unsigned short
* @param string
*/
void Client::send(unsigned short type, std::string message)
{
	MessageObject m(type, message);
	send(m);
}

/**
* recieves a MessageObject from the server
* @return MessageObject
*/
MessageObject Client::recieve()
{
	MessageObject m;
	sf::Packet packet;
	server.receive(packet);
	packet >> m;
	return m;
}

/**
* shuts down the client, and sends a disconnect message to the server
*/
void Client::shutDown()
{
	std::cout << "Connection closed\nPress Enter to exit";
	send(MessageObject::CMD, "DISC");
	isRunning = false;
	server.disconnect();
}

/**
* destructor, if the client is still running, shuts it down
*/
Client::~Client()
{
	if (isRunning)
		shutDown();
	//delete player;
	delete map;
}