//
//  game_result.hpp
//  BlockSim
//
//  Created by Harry Kalodner on 7/1/16.
//  Copyright © 2016 Harry Kalodner. All rights reserved.
//

#ifndef game_result_hpp
#define game_result_hpp

#include "typeDefs.hpp"

#include <map>

struct MinerResult;
class Miner;

struct GameResult {
    std::map<const Miner *, MinerResult> minerResults;
    BlockCount totalBlocksMined;
    BlockCount blocksInLongestChain;
    Value moneyLeftAtEnd;
    
    GameResult(std::map<const Miner *, MinerResult> minerResults, BlockCount totalBlocksMined, BlockCount blocksInLongestChain, Value moneyLeftAtEnd);
};

#endif /* game_result_hpp */
