#include "pch.h"
#include "ClientPacketHandler.h"
#include "Player.h"
#include "Room.h"
#include "GameSession.h"

PacketHandlerFunc GPacketHandler[UINT16_MAX];

// 직접 컨텐츠 작업자

bool Handle_INVALID(PacketSessionRef& session, BYTE* buffer, int32 len)
{
	PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);
	// TODO : Log
	return false;
}

bool Handle_C_LOGIN(PacketSessionRef& session, Protocol::C_LOGIN& pkt)
{
	GameSessionRef gameSession = static_pointer_cast<GameSession>(session);

	// TODO : Validation 체크

	Protocol::S_LOGIN loginPkt;
	loginPkt.set_success(true);

	// DB에서 플레이 정보를 긁어온다
	// GameSession에 플레이 정보를 저장 (메모리)

	// ID 발급 (DB 아이디가 아니고, 인게임 아이디)
	static Atomic<uint64> idGenerator = 1;

	{
		auto player = loginPkt.mutable_player();
		player->set_id(idGenerator);
		player->set_name(pkt.name());

		PlayerRef playerRef = MakeShared<Player>();
		playerRef->playerId = idGenerator++;
		playerRef->name = pkt.name();
		playerRef->ownerSession = gameSession;

		gameSession->AddPlayer(playerRef);
		GRoom.Enter(playerRef); 
	}


	auto sendBuffer = ClientPacketHandler::MakeSendBuffer(loginPkt);
	session->Send(sendBuffer);

	return true;
}

bool Handle_C_COMMAND(PacketSessionRef& session, Protocol::C_COMMAND& pkt)
{
	std::cout << "Command" << pkt.msg() << endl;

	GameSessionRef gameSession = static_pointer_cast<GameSession>(session);
	PlayerRef player = gameSession->GetPlayer();

	string str = pkt.msg();
	string command = str.substr(1);

	Protocol::S_COMMAND commandPkt;
	if (command.find("all") != string::npos)
	{
		player->chatType = Protocol::CHAT_TYPE_ALL;
		//commandPkt.set_chatType(Protocol::CHAT_TYPE_ALL);
	}
	else if (command.find("ch") != string::npos)
	{
		player->chatType = Protocol::CHAT_TYPE_CHANNEL;
		player->channelNum = str.back() - '0';
		//commandPkt.set_chatType(Protocol::CHAT_TYPE_CHANNEL);
		commandPkt.set_channel(str.back() - '0');
	}
	else
	{
		cout << "Bad Command" << endl;
		return false;
	}

	auto sendBuffer = ClientPacketHandler::MakeSendBuffer(commandPkt);

	return true;
}


bool Handle_C_CHAT(PacketSessionRef& session, Protocol::C_CHAT& pkt)
{
	std::cout << pkt.msg() << endl;

	string str = pkt.msg();

	Protocol::S_CHAT chatPkt;
	chatPkt.set_msg(pkt.msg());
	chatPkt.set_chattype(pkt.chattype());
	auto sendBuffer = ClientPacketHandler::MakeSendBuffer(chatPkt);

	switch (pkt.chattype())
	{
	case Protocol::CHAT_TYPE_ALL:
		GRoom.Broadcast(sendBuffer); // WRITE_LOCK
		break;
	case Protocol::CHAT_TYPE_CHANNEL:
		GRoom.BroadcastChannel(sendBuffer, pkt.channel());
		break;
	default:
		cout << "Channel Type is None" << endl;
		GRoom.Broadcast(sendBuffer);
		break;
	}


	return true;
}
