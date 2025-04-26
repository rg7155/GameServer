#include "pch.h"
#include "ThreadManager.h"
#include "Service.h"
#include "Session.h"
#include "BufferReader.h"
#include "ServerPacketHandler.h"
#include "ServerSession.h"
#include "Player.h"

//빌드 전 이벤트 
//CALL $(SolutionDir)Common\Protobuf\bin\GenPackets.bat



PlayerRef GPlayer;
ClientServiceRef GService;

void CheckCommand(const string& str)
{
	Protocol::C_COMMAND commandPkt;
	//auto player = commandPkt.player(); //읽기 전용
	auto player = commandPkt.mutable_player(); //쓰기 전용

	string command = str.substr(1);
	auto type = Protocol::CHAT_TYPE_ALL;
	auto channel = 0;

	if (command.find("all") != string::npos)
	{
		type = Protocol::CHAT_TYPE_ALL;
	}
	else if (command.find("ch") != string::npos)
	{
		type = Protocol::CHAT_TYPE_CHANNEL;
		channel = str.back() - '0';
	}
	player->set_chattype(type);
	player->set_channel(channel);
	auto sendBuffer = ServerPacketHandler::MakeSendBuffer(commandPkt);
	GService->Broadcast(sendBuffer);
}
void CheckChat(const string& str)
{
	ServerSessionRef serverSession = static_pointer_cast<ServerSession>(GService->GetSession());
	PlayerRef playerRef = serverSession->GetPlayer();

	Protocol::C_CHAT chatPkt;
	chatPkt.set_msg(str);

	auto playerPkt = chatPkt.mutable_player(); //쓰기 전용
	//chatPkt.set_chattype(GPlayer->chatType);
	playerPkt->set_id(playerRef->playerId);
	playerPkt->set_name(playerRef->name);
	playerPkt->set_chattype(playerRef->chatType);
	playerPkt->set_channel(playerRef->channelNum);

	auto sendBuffer = ServerPacketHandler::MakeSendBuffer(chatPkt);
	GService->Broadcast(sendBuffer);
}

void CheckTestChat()
{
	ServerSessionRef serverSession = static_pointer_cast<ServerSession>(GService->GetSession());
	PlayerRef playerRef = serverSession->GetPlayer();

	Protocol::C_CHAT chatPkt;
	chatPkt.set_msg("Hello!");

	auto playerPkt = chatPkt.mutable_player(); //쓰기 전용
	//chatPkt.set_chattype(GPlayer->chatType);
	playerPkt->set_id(playerRef->playerId);
	playerPkt->set_name(playerRef->name);
	playerPkt->set_chattype(playerRef->chatType);
	playerPkt->set_channel(playerRef->channelNum);

	auto sendBuffer = ServerPacketHandler::MakeSendBuffer(chatPkt);
	GService->Broadcast(sendBuffer);
}

int main()
{
	ServerPacketHandler::Init();
	//GPlayer = MakeShared<Player>();
	//GPlayer->chatType = Protocol::CHAT_TYPE_ALL;

	this_thread::sleep_for(1s);

	uint64 sessionCount = 0;
	cout << "Input SessionCount:";
	cin >> sessionCount;

	//TODO
	bool isTestMode = sessionCount > 1;
	if (!isTestMode)
	{
		GService = MakeShared<ClientService>(
			NetAddress(L"127.0.0.1", 7777),
			MakeShared<IocpCore>(),
			MakeShared<ServerSession>,
			sessionCount);
	}
	else
	{
		GService = MakeShared<ClientService>(
			NetAddress(L"127.0.0.1", 7777),
			MakeShared<IocpCore>(),
			MakeShared<ServerTestSession>,
			sessionCount);
	}

	ASSERT_CRASH(GService->Start());

	for (int32 i = 0; i < 2; i++)
	{
		GThreadManager->Launch([=]()
		{
			while (true)
			{
				GService->GetIocpCore()->Dispatch();
			}
		});
	}

	//TODO connected 이후
	this_thread::sleep_for(1s);

	while (true)
	{
		if (isTestMode)
		{
			string str = "Test_Msg";
			str += (to_string(rand() % 100));
			CheckChat(str);
			this_thread::sleep_for(1s);
		}
		else
		{
			string str = "";
			cin >> str;
			if (str[0] == '/')
			{
				CheckCommand(str);
			}
			else
			{
				CheckChat(str);
			}
		}

	}

	GThreadManager->Join();
}

