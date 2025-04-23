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
	//std::cout << "Command : " << pkt.msg() << endl;

	//GameSessionRef gameSession = static_pointer_cast<GameSession>(session);
	//PlayerRef player = gameSession->GetPlayer();

	auto player = pkt.player();

	Protocol::S_COMMAND commandPkt;
	if (player.chattype() == Protocol::CHAT_TYPE_ALL)
	{
		//player->chatType = Protocol::CHAT_TYPE_ALL;
		player.set_chattype(Protocol::CHAT_TYPE_ALL);
	}
	else if (player.chattype() == Protocol::CHAT_TYPE_CHANNEL)
	{
		//player->chatType = Protocol::CHAT_TYPE_CHANNEL;
		//player->channelNum = player.channel();
		player.set_chattype(Protocol::CHAT_TYPE_CHANNEL);
		player.set_channel(player.channel());
	}
	else
	{
		cout << "Bad Command" << endl;
		return false;
	}

	auto sendBuffer = ClientPacketHandler::MakeSendBuffer(commandPkt);
	session->Send(sendBuffer);
	return true;
}


bool Handle_C_CHAT(PacketSessionRef& session, Protocol::C_CHAT& pkt)
{
	std::cout << pkt.msg() << endl;

	string str = pkt.msg();

	//GameSessionRef gameSession = static_pointer_cast<GameSession>(session);
	//PlayerRef player = gameSession->GetPlayer();
	auto player = pkt.player();

	Protocol::S_CHAT chatPkt;
	chatPkt.set_msg(pkt.msg());
	chatPkt.mutable_player()->CopyFrom(pkt.player()); //  protobuf 메시지끼리의 deep copy 함수
	auto sendBuffer = ClientPacketHandler::MakeSendBuffer(chatPkt);

	switch (player.chattype())
	{
	case Protocol::CHAT_TYPE_ALL:
		GRoom.Broadcast(sendBuffer); // WRITE_LOCK
		break;
	case Protocol::CHAT_TYPE_CHANNEL:
		GRoom.BroadcastChannel(sendBuffer, player.channel());
		break;
	default:
		cout << "Channel Type is None" << endl;
		GRoom.Broadcast(sendBuffer);
		break;
	}
	//std::cout << "Recv " << chatType << "/" << player.name() << endl;


	return true;
}
