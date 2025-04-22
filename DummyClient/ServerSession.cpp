#include "pch.h"
#include "ServerSession.h"
#include "ServerPacketHandler.h"

void ServerSession::OnConnected()
{
	cout << " Server Connected" << endl;
	cout << "What's your name? >>";

	Protocol::C_LOGIN pkt;
	string name;
	cin >> name;
	pkt.set_name(name);

	auto sendBuffer = ServerPacketHandler::MakeSendBuffer(pkt);
	Send(sendBuffer);
}

void ServerSession::OnRecvPacket(BYTE* buffer, int32 len)
{
	PacketSessionRef session = GetPacketSessionRef();
	PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);

	// TODO : packetId 대역 체크
	ServerPacketHandler::HandlePacket(session, buffer, len);
}

void ServerSession::OnSend(int32 len)
{
	//cout << "OnSend Len = " << len << endl;
}

void ServerSession::OnDisconnected()
{
	//cout << "Disconnected" << endl;
}
