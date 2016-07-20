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

#include <iostream>

class Blockchain;
struct GameResult;
class MinerGroup;

std::pair< std::unique_ptr<Blockchain>, GameResult> runGame(MinerGroup &minerGroup, BlockCount numberOfBlocks, BlockRate secondsPerBlock, ValueRate transactionFeeRate);


#endif /* game_hpp */
