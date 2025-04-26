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
	pkt.set_testclient(false);

	auto sendBuffer = ServerPacketHandler::MakeSendBuffer(pkt);
	Send(sendBuffer);
}

void ServerSession::OnRecvPacket(BYTE* buffer, int32 len)
{
	PacketSessionRef session = GetPacketSessionRef();
	PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);

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

/// <summary>
/// ///////////////////////////////////////
/// </summary>
/// 
void ServerTestSession::OnConnected()
{
	Protocol::C_LOGIN pkt;
	string name = "Test_Name";
	pkt.set_name(name);
	pkt.set_testclient(true);

	auto sendBuffer = ServerPacketHandler::MakeSendBuffer(pkt);
	Send(sendBuffer);
}

void ServerTestSession::OnRecvPacket(BYTE* buffer, int32 len)
{
	PacketSessionRef session = GetPacketSessionRef();
	PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);

	ServerPacketHandler::HandlePacket(session, buffer, len);
}

void ServerTestSession::OnSend(int32 len)
{
}

void ServerTestSession::OnDisconnected()
{
}
