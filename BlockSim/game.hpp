//
//  game.hpp
//  BlockSim
//
//  Created by Harry Kalodner on 6/6/16.
//  Copyright © 2016 Harry Kalodner. All rights reserved.
//

#ifndef game_hpp
#define game_hpp

#include "typeDefs.hpp"
#include "blockchain.hpp"

#include <iostream>

class Blockchain;
struct GameResult;
class MinerGroup;
struct BlockchainSettings;

struct GameSettings {
    BlockCount numberOfBlocks;
    BlockchainSettings blockchainSettings;
};

std::pair< std::unique_ptr<Blockchain>, GameResult> runGame(MinerGroup &minerGroup, GameSettings gameSettings);

#endif /* game_hpp */
