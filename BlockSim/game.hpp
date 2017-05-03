//
//  game.hpp
//  BlockSim
//
//  Created by Harry Kalodner on 6/6/16.
//  Copyright © 2016 Harry Kalodner. All rights reserved.
//

#ifndef game_hpp
#define game_hpp

#include "blockchain_settings.hpp"
#include "typeDefs.hpp"

class Blockchain;
struct GameResult;
class MinerGroup;
struct BlockchainSettings;

struct GameSettings {
    BlockchainSettings blockchainSettings;
};

GameResult runGame(MinerGroup &minerGroup, Blockchain &blockchain, GameSettings gameSettings);

#endif /* game_hpp */
