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
		//playerRef->ownerSession = gameSession;

		serverSession->_players.push_back(playerRef);
	}
	return true;
}

bool Handle_S_COMMAND(PacketSessionRef& session, Protocol::S_COMMAND& pkt)
{
	//TODO 채널 타입,채널 셋팅
	return false;
}


bool Handle_S_CHAT(PacketSessionRef& session, Protocol::S_CHAT& pkt)
{
	string chatType = "";
	switch (pkt.chattype())
	{
	case Protocol::CHAT_TYPE_ALL:
		chatType = "All";
		break;
	case Protocol::CHAT_TYPE_CHANNEL:
		chatType = "Ch" + pkt.channel();
		break;
	default:
		break;
	}
	std::cout << "[" << chatType << "/" << "Name" << "]" << pkt.msg() << endl;
	return true;
}

