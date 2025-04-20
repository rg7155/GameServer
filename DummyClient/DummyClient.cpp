#include "pch.h"
#include "ThreadManager.h"
#include "Service.h"
#include "Session.h"
#include "BufferReader.h"
#include "ServerPacketHandler.h"

//빌드 전 이벤트 
//CALL $(SolutionDir)Common\Protobuf\bin\GenPackets.bat
class ServerSession : public PacketSession
{
public:
	~ServerSession()
	{
		cout << "~ServerSession" << endl;
	}

	virtual void OnConnected() override
	{
		cout << " Server Connected" << endl;
		Protocol::C_LOGIN pkt;
		auto sendBuffer = ServerPacketHandler::MakeSendBuffer(pkt);
		Send(sendBuffer);
	}

	virtual void OnRecvPacket(BYTE* buffer, int32 len) override
	{
		PacketSessionRef session = GetPacketSessionRef();
		PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);

		// TODO : packetId 대역 체크
		ServerPacketHandler::HandlePacket(session, buffer, len);
	}

	virtual void OnSend(int32 len) override
	{
		//cout << "OnSend Len = " << len << endl;
	}

	virtual void OnDisconnected() override
	{
		//cout << "Disconnected" << endl;
	}
};

//TODO
class Player
{
public:

	//uint64					playerId = 0;
	//string					name;
	//Protocol::PlayerType	type = Protocol::PLAYER_TYPE_NONE;
	Protocol::ChatType		chatType = Protocol::CHAT_TYPE_NONE;
	uint32					channelNum = 0;

};


shared_ptr<class Player> GPlayer;
ClientServiceRef GService;

void CheckCommand(const string& str)
{
	string command = str.substr(1);
	if (command.find("all") != string::npos)
	{
		GPlayer->chatType = Protocol::CHAT_TYPE_ALL;
		cout << "Change All Mode" << endl;
	}
	else if (command.find("ch") != string::npos)
	{
		GPlayer->chatType = Protocol::CHAT_TYPE_CHANNEL;
		GPlayer->channelNum = str.back() - '0';
		cout << "Change Ch" << GPlayer->channelNum << " Mode" << endl;
	}
}
void CheckChat(const string& str)
{
	Protocol::C_CHAT chatPkt;
	chatPkt.set_msg(str);
	chatPkt.set_chattype(GPlayer->chatType);

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

