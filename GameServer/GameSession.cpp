#include "pch.h"
#include "GameSession.h"
#include "GameSessionManager.h"
#include "ClientPacketHandler.h"
#include "Player.h"

void GameSession::OnConnected()
{
	GSessionManager.Add(static_pointer_cast<GameSession>(shared_from_this()));
}

void GameSession::OnDisconnected()
{
	cout << "Disconnect ID : " << _player->playerId << endl;
	GSessionManager.Remove(static_pointer_cast<GameSession>(shared_from_this()));
}

void GameSession::OnRecvPacket(BYTE* buffer, int32 len)
{
	PacketSessionRef session = GetPacketSessionRef();
	PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);

	// TODO : packetId 대역 체크
	ClientPacketHandler::HandlePacket(session, buffer, len);
}

void GameSession::OnSend(int32 len)
{
}

void GameSession::AddPlayer(PlayerRef player)
{
	//_players.push_back(player);
	_player = player;
}

PlayerRef GameSession::GetPlayer()
{
	return _player;
	/*for (auto& p : _players)
	{
		if (p->playerId == playerId)
			return p;
	}
	return nullptr;*/
}
