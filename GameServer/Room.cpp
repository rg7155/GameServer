#include "pch.h"
#include "Room.h"
#include "Player.h"
#include "GameSession.h"

Room GRoom;

void Room::Enter(PlayerRef player)
{
	WRITE_LOCK;
	_players[player->playerId] = player;
}

void Room::Leave(PlayerRef player)
{
	WRITE_LOCK;
	_players.erase(player->playerId);
}

void Room::Broadcast(SendBufferRef sendBuffer)
{
	WRITE_LOCK;
	for (auto& p : _players)
	{
		p.second->ownerSession->Send(sendBuffer);
	}
}

void Room::BroadcastChannel(SendBufferRef sendBuffer, uint64 channel)
{
	WRITE_LOCK;
	for (auto& p : _players)
	{
		if (p.second->chatType != Protocol::CHAT_TYPE_CHANNEL)
			continue;
		//cout << "BroadcastChannel id:" << p.second->playerId << "channelNum:" << p.second->channelNum << "chatType:" << p.second->chatType << endl;
		if(p.second->channelNum == channel)
			p.second->ownerSession->Send(sendBuffer);
	}
}
