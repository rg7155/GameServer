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
	commandPkt.set_msg(str);

	//string command = str.substr(1);
	//if (command.find("all") != string::npos)
	//{
	//	commandPkt.set_chattype(Protocol::CHAT_TYPE_ALL);
	//	//cout << "Change All Mode" << endl;
	//}
	//else if (command.find("ch") != string::npos)
	//{
	//	commandPkt.set_chattype(Protocol::CHAT_TYPE_CHANNEL);
	//	commandPkt.set_channel(str.back() - '0');
	//	//cout << "Change Ch" << GPlayer->channelNum << " Mode" << endl;
	//}

	auto sendBuffer = ServerPacketHandler::MakeSendBuffer(commandPkt);
	GService->Broadcast(sendBuffer);
}
void CheckChat(const string& str)
{
	Protocol::C_CHAT chatPkt;
	chatPkt.set_msg(str);
	//chatPkt.set_chattype(GPlayer->chatType);

	auto sendBuffer = ServerPacketHandler::MakeSendBuffer(chatPkt);
	GService->Broadcast(sendBuffer);
}

int main()
{
	ServerPacketHandler::Init();
	GPlayer = MakeShared<Player>();
	GPlayer->chatType = Protocol::CHAT_TYPE_ALL;

	this_thread::sleep_for(1s);

	GService = MakeShared<ClientService>(
		NetAddress(L"127.0.0.1", 7777),
		MakeShared<IocpCore>(),
		MakeShared<ServerSession>, 
		1);

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
		string str = "";
		//cout << ">>";
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

	GThreadManager->Join();
}

