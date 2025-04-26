#pragma once
#include "Session.h"

class ServerSession : public PacketSession
{
public:
	~ServerSession()
	{
		cout << "~ServerSession" << endl;
	}

	virtual void OnConnected() override;
	virtual void OnRecvPacket(BYTE* buffer, int32 len) override;
	virtual void OnSend(int32 len) override;
	virtual void OnDisconnected() override;

	PlayerRef	GetPlayer() { return _player; };
	void		SetPlayer(PlayerRef player) { _player = player; };

private:
	//Vector<PlayerRef> _players;
	PlayerRef _player;

};

class ServerTestSession : public PacketSession
{
public:
	~ServerTestSession()
	{
		cout << "~ServerTestSession" << endl;
	}

	virtual void OnConnected() override;
	virtual void OnRecvPacket(BYTE* buffer, int32 len) override;
	virtual void OnSend(int32 len) override;
	virtual void OnDisconnected() override;

	PlayerRef	GetPlayer() { return _player; };
	void		SetPlayer(PlayerRef player) { _player = player; };

private:
	PlayerRef _player;

};