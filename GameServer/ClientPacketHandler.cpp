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

	Protocol::S_LOGIN loginPkt;
	loginPkt.set_success(true);


	static Atomic<uint64> idGenerator = 0;

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

	//log
	{
		cout << "=======LOGIN=======" << endl;
		cout << "ID:" << idGenerator << endl;
		cout << "name:" << pkt.name() << endl;
		cout << "==============" << endl;
	}

	return true;
}

bool Handle_C_COMMAND(PacketSessionRef& session, Protocol::C_COMMAND& pkt)
{
	//log
	{
		auto player = pkt.player();
		cout << "=======COMMAND=======" << endl;
		//cout << "ID:" << player.id() << "name:" <<player.name() << "chatType:" << player.chattype() << "channelNum:" << player.channel() << "msg:" << pkt.msg() << endl;
		cout << "ID:" << player.id() << endl;
		cout << "name:" << player.name() << endl;
		cout << "chatType:" << player.chattype() << endl;
		cout << "channelNum:" << player.channel() << endl;
		cout << "==============" << endl;
	}

	//GameSessionRef gameSession = static_pointer_cast<GameSession>(session);
	//PlayerRef player = gameSession->GetPlayer();

	auto player = pkt.player();

	Protocol::S_COMMAND commandPkt;
	commandPkt.mutable_player()->CopyFrom(pkt.player()); //  protobuf 메시지끼리의 deep copy 함수

	GameSessionRef gameSession = static_pointer_cast<GameSession>(session);
	PlayerRef playerRef = gameSession->GetPlayer();
	if (player.chattype() == Protocol::CHAT_TYPE_ALL)
	{
		playerRef->chatType = player.chattype();
	}
	else if (player.chattype() == Protocol::CHAT_TYPE_CHANNEL)
	{
		playerRef->chatType = player.chattype();
		playerRef->channelNum = player.channel();
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
	//log
	{
		auto player = pkt.player();
		cout << "=======CHAT=======" << endl;
		//cout << "ID:" << player.id() << "name:" <<player.name() << "chatType:" << player.chattype() << "channelNum:" << player.channel() << "msg:" << pkt.msg() << endl;
		cout << "ID:" << player.id() << endl;
		cout << "name:" << player.name() << endl;
		cout << "chatType:" << player.chattype() << endl;
		cout << "channelNum:" << player.channel() << endl;
		cout << "msg:" << pkt.msg() << endl;
		cout << "==============" << endl;
	}

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
