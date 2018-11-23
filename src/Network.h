#pragma once

#include <SDL_net.h>
#include <time.h>
#include <chrono>
#include <vector>

#include "../Osa.h"

struct Client {
	bool		isFree = true;
	IPaddress	ip;
};

struct ClientMessage {
	IPaddress ip;
	std::string data;
};

class Network {
public:
	std::string			serverName = "localhost";
	short				PORT = 9999;
	bool				isServer = false;
	const int32_t		PACKET_SIZE = 512;
	int					PACKET_AMT = 200;
	bool				DEBUG = false;

	int					MAX_CLIENTS = 100;
	Client*				clients[100];
	int					clientCount = 0;

	const std::string	MESSAGE_JOIN = "JOIN";
	const std::string	MESSAGE_PING = "PING";
	const std::string	MESSAGE_PEW = "PEW";
	const std::string	MESSAGE_FULL = "FULL";
	const std::string	MESSAGE_ACCEPTED = "ACCEPTED";
	const std::string	MESSAGE_ALREADYJOINED = "ALREADYJOINED";
	const std::string	MESSAGE_DISCONNECTED = "DISCONNECTED";

	UDPsocket			UDPSocket;	//Local Socket
	IPaddress			ip;			//Server IP

	//Data (not needed but is cool to display)
	int	totalPacketsSent = 0, totalPacketsRecv = 0;
	int totalBytesSent = 0, totalBytesRecv = 0;
	int timedPacketsSent = 0, timedPacketsRecv = 0;
	int timedBytesSent = 0, timedBytesRecv = 0;
	int lastPacketsSent = 0, lastPacketsRecv = 0;
	int lastBytesSent = 0, lastBytesRecv = 0;
	time_t start;
	long long ping;
	std::chrono::time_point<std::chrono::steady_clock> pingTimer;

	Network();

	void init(Osa osa);
	std::vector<ClientMessage> update();

	void sendMessage(std::string message, IPaddress _ip);
	UDPpacket* allocPacket(int32_t size);
	UDPpacket** allocPackets(int32_t amt, int32_t size);
	void refreshIP();
	void refreshPing(IPaddress ip);
	bool isIPRegistered(IPaddress ip);
	int getIPLoc(IPaddress ip);
	void kickIP(IPaddress ip);
};