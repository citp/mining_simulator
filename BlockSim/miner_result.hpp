//
//  miner_result.hpp
//  BlockSim
//
//  Created by Harry Kalodner on 6/18/16.
//  Copyright © 2016 Harry Kalodner. All rights reserved.
//

#ifndef miner_result_hpp
#define miner_result_hpp

#include "typeDefs.hpp"

class Block;

struct MinerResult {
    BlockCount blocksInWinningChain;
    Value totalProfit;
    
    MinerResult();
    void addBlock(const Block *block);
};

#endif /* miner_result_hpp */
