#pragma once

#include <vector>
#include <iostream>
#include "Block.h"
#include "Player.h"

class Map
{

    friend std::ostream& operator<<(std::ostream& o, const Map& map);
    friend std::istream& operator>>(std::istream& i, Map& map);

public:

    Map();
    Map(const short& sizeX, const short& sizeY, std::vector<const Block*> blocks = std::vector<const Block*>());
    ~Map();

    const std::vector<const Block*>& getBlocks() const;
    const std::vector<Player*>& getPlayers() const;
    const short& getSizeX() const;
    const short& getSizeY() const;

    void add(const Block* const block);
    void add(Player* player);
	void addNewPlayer(Player* player);
	void updatePlayer(Player* const player);

	const AbstractEntity* isEntityOnPosition(const AbstractEntity* entity, const short& entityPosX, const short& entityPosY, const short& posX, const short& posY, const short& bounds) const;
	Tank* getTankOnPosition(const short& posX, const short& posY, Tank* thisTank) const;
	const Block* getBlockOnPosition(const short& posX, const short& posY) const;

private:

    short sizeX, sizeY;

    std::vector<const Block*> blocks;
    std::vector<Player*> players;

	static short BLOCKS_COUNT;

    void initBlocks();
};