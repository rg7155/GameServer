#pragma once

class Player
{
public:

	uint64					playerId = 0;
	string					name;
	GameSessionRef			ownerSession; // Cycle
	Protocol::ChatType		chatType = Protocol::CHAT_TYPE_NONE;
	uint32					channelNum = 0;
};

