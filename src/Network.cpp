#include "Network.h"

Network::Network(){}

//If this is being used by itself you need to do SDLNet_INIT!
void Network::init(Osa osa) {
	isServer = osa.isServer(); //Dont forget to change for server if needed!
	start = time(0);
	if (isServer) { //Server INIT
		std::cout << "Network: INIT Server" << std::endl;
		std::cout << "Network: Opening port: " << PORT << std::endl;
		UDPSocket = SDLNet_UDP_Open(PORT);
		if (UDPSocket == nullptr)
			std::cout << "Error: Network::init: SDLNet_UDP_Open: " << SDLNet_GetError() << std::endl;
		for (int i = 0; i < MAX_CLIENTS; i++) {
			clients[i] = new Client();
			clients[i]->isFree = true;
		}
		std::cout << "Network: INIT Done!" << std::endl;
		std::cout << "Network: Waiting For Clients" << std::endl;

	} else { //Client INIT
		std::cout << "Network: INIT Client" << std::endl;
		std::cout << "Network: Opening port: " << PORT << std::endl;
		short port = PORT;
		UDPSocket = SDLNet_UDP_Open(PORT);
		while (UDPSocket == nullptr) {
			std::cout << "Error: Network::init: SDLNet_UDP_Open: " << SDLNet_GetError() << std::endl;
			PORT--;
			refreshIP();
			UDPSocket = SDLNet_UDP_Open(PORT);
		}
		std::cout << "Network: Port Opened at: " << PORT << std::endl;
		PORT = port;
		std::cout << "Network: Set port back to: " << PORT << std::endl;
		refreshIP();
	}
}

std::vector<ClientMessage> Network::update() {
	std::vector<ClientMessage> cms;
	if (difftime(time(0), start) >= 1) {
		lastPacketsSent = timedPacketsSent;
		lastPacketsRecv = timedPacketsRecv;
		lastBytesSent = timedBytesSent;
		lastBytesRecv = timedBytesRecv;

		timedPacketsSent = 0;
		timedPacketsRecv = 0;
		timedBytesSent = 0;
		timedBytesRecv = 0;
	}

	UDPpacket** packets = allocPackets(PACKET_AMT, PACKET_SIZE);
	int amt = SDLNet_UDP_RecvV(UDPSocket, packets);
	if (amt == -1)
		std::cout << "Error: Network::update: " << SDLNet_GetError() << std::endl;
	int length = (sizeof(packets) / sizeof(*packets));
	if (amt > 0) {
		for (int i = 0; i < amt; i++) {
			//Handle Messages
			std::string dataa = (char*) packets[i]->data;
			std::string data = dataa.substr(0, packets[i]->len);
			totalBytesRecv += data.size();
			totalPacketsRecv += 1;
			timedBytesRecv += data.size();
			timedPacketsRecv += 1;
			if (data == MESSAGE_PING)
				sendMessage(MESSAGE_PEW, packets[i]->address);
			else if (data == MESSAGE_PEW) {
				std::chrono::time_point<std::chrono::steady_clock> end = std::chrono::high_resolution_clock::now();
				std::chrono::nanoseconds dur = end - pingTimer;
				ping = std::chrono::duration_cast<std::chrono::milliseconds>(dur).count();
				if (DEBUG)
					std::cout << "PING: " << ping << std::endl;
			}
			if (isServer) {
				if (data == MESSAGE_JOIN) {
					if (!isIPRegistered(packets[i]->address)) {
						if (clientCount < MAX_CLIENTS) {
							for (int ii = 0; ii < MAX_CLIENTS; ii++) {
								if (clients[ii]->isFree == true) {
									clients[ii]->isFree = false;
									clients[ii]->ip = packets[i]->address;
									clientCount++;
									sendMessage(MESSAGE_ACCEPTED, clients[ii]->ip);
									std::cout << "Player connected! " << clientCount << " players connected!" << std::endl;
									break;
								}
							}
						} else {
							sendMessage(MESSAGE_FULL, packets[i]->address);
						}
					} else {
						sendMessage(MESSAGE_ALREADYJOINED, packets[i]->address);
					}
				} else if (data == MESSAGE_DISCONNECTED) {
					kickIP(packets[i]->address);
				}
			}

			if(DEBUG)
				std::cout << "Incoming Packet with data: " << data << std::endl << "Total Packets: " << totalPacketsRecv << ". Total Bytes: " << totalBytesRecv << std::endl;
			packets[i]->data = (Uint8*)"";
			
			ClientMessage cm;
			cm.ip = packets[i].address;
			cm.data = data;
			cms.push_back(cm);
		}
	}
	SDLNet_FreePacketV(packets);
	
	return cms;
}

void Network::sendMessage(std::string message, IPaddress _ip) {
	totalBytesSent += message.size();
	totalBytesSent += 1;
	timedBytesSent += message.size();
	timedBytesSent += 1;

	UDPpacket* packet = allocPacket(PACKET_SIZE);

	packet->address.host = _ip.host;
	packet->address.port = _ip.port;

	packet->data = (Uint8*) message.c_str();
	packet->len = message.length();

	if (SDLNet_UDP_Send(UDPSocket, -1, packet) == 0)
		std::cout << "Error: Network::sendMessage: " << SDLNet_GetError() << std::endl;
	SDLNet_FreePacket(packet);
}

UDPpacket* Network::allocPacket(int32_t size) {
	UDPpacket* pack = SDLNet_AllocPacket(size);
	if (pack == nullptr)
		std::cout << "Error: Network::allocPacket: " << SDLNet_GetError() << std::endl;
	return pack;
}

UDPpacket** Network::allocPackets(int32_t amt, int32_t size) {
	UDPpacket** pack = SDLNet_AllocPacketV(amt, size);
	if(pack == nullptr)
		std::cout << "Error: Network::allocPackets: " << SDLNet_GetError() << std::endl;
	return pack;
}

void Network::refreshIP() {
	if (SDLNet_ResolveHost(&ip, serverName.c_str(), PORT) == -1) {
		std::cout << "Error: Network::refreshIP: " << SDLNet_GetError() << std::endl;
	}
}

void Network::refreshPing(IPaddress ip) {
	sendMessage(MESSAGE_PING, ip);
	pingTimer = std::chrono::high_resolution_clock::now();
}

bool Network::isIPRegistered(IPaddress ip) {
	for (int i = 0; i < MAX_CLIENTS; i++) {
		if (!clients[i]->isFree) {
			if (clients[i]->ip.host == ip.host && clients[i]->ip.port == ip.port) {
				return true;
			}
		}
	}
	return false;
}

int Network::getIPLoc(IPaddress ip) {
	for (int i = 0; i < MAX_CLIENTS; i++)
		if (!clients[i]->isFree)
			if (clients[i]->ip.host == ip.host && clients[i]->ip.port == ip.port)
				return i;
	return -1;
}

void Network::kickIP(IPaddress ip) {
	if (isIPRegistered(ip)) {
		int i = getIPLoc(ip);
		clients[i]->isFree = false;
		clients[i]->ip = IPaddress();
		clientCount--;
		std::cout << "Player disconnected! " << clientCount << " players connected!" << std::endl;
	}
}