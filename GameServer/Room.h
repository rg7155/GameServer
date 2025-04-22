#pragma once

class Room
{
public:
	void Enter(PlayerRef player);
	void Leave(PlayerRef player);
	void Broadcast(SendBufferRef sendBuffer);
	void BroadcastChannel(SendBufferRef sendBuffer, uint64 channel);

private:
	USE_LOCK;
	map<uint64, PlayerRef> _players;
};

extern Room GRoom;