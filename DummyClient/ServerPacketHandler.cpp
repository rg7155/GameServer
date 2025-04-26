#include "pch.h"
#include "ServerPacketHandler.h"
#include "ServerSession.h"
#include "Player.h"

PacketHandlerFunc GPacketHandler[UINT16_MAX];

// 직접 컨텐츠 작업자

bool Handle_INVALID(PacketSessionRef& session, BYTE* buffer, int32 len)
{
	PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);
	// TODO : Log
	return false;
}

bool Handle_S_LOGIN(PacketSessionRef& session, Protocol::S_LOGIN& pkt)
{
	if (pkt.success() == false)
		return true;

	ServerSessionRef serverSession = static_pointer_cast<ServerSession>(session);
	{
		PlayerRef playerRef = MakeShared<Player>();
		auto player = pkt.player();
		playerRef->playerId = player.id();
		playerRef->name = player.name();
		playerRef->chatType = Protocol::CHAT_TYPE_ALL;
		playerRef->channelNum = 0;
		//playerRef->ownerSession = gameSession;

		//serverSession->_players.push_back(playerRef);
		serverSession->SetPlayer(playerRef);
	}
	return true;
}

bool Handle_S_COMMAND(PacketSessionRef& session, Protocol::S_COMMAND& pkt)
{
	//TODO 채널 타입,채널 셋팅
	//ServerSessionRef serverSession = static_pointer_cast<ServerSession>(session);
	//PlayerRef player = serverSession->GetPlayer();
	//player->chatType = pkt.chattype();
	//player->channelNum = pkt.channel();

	auto player = pkt.player();

	ServerSessionRef serverSession = static_pointer_cast<ServerSession>(session);
	PlayerRef playerRef = serverSession->GetPlayer();

	string chatType = "";
	switch (player.chattype())
	{
	case Protocol::CHAT_TYPE_ALL:
		chatType = "All";
		playerRef->chatType = player.chattype();
		break;
	case Protocol::CHAT_TYPE_CHANNEL:
		chatType = "Ch" + player.channel();
		playerRef->chatType = player.chattype();
		playerRef->channelNum = player.channel();
		break;
	default:
		break;
	}
	std::cout << "Change Channel To " << chatType << endl;

	return true;
}


bool Handle_S_CHAT(PacketSessionRef& session, Protocol::S_CHAT& pkt)
{
	//ServerSessionRef serverSession = static_pointer_cast<ServerSession>(session);
	//PlayerRef player = serverSession->GetPlayer(); 

	auto player = pkt.player();

	string chatType = "";
	switch (player.chattype())
	{
	case Protocol::CHAT_TYPE_ALL:
		chatType = "All";
		break;
	case Protocol::CHAT_TYPE_CHANNEL:
		chatType = "Ch" + to_string(player.channel());
		break;
	default:
		break;
	}
	std::cout << "[" << chatType << "/" << player.name() << "]" << pkt.msg() << endl;
	return true;
}

