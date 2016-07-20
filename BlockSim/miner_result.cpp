//
//  miner_result.cpp
//  BlockSim
//
//  Created by Harry Kalodner on 6/18/16.
//  Copyright © 2016 Harry Kalodner. All rights reserved.
//

#include "miner_result.hpp"

#include "block.hpp"

MinerResult::MinerResult() : blocksInWinningChain(BlockCount(0)), totalProfit(Value(0)) {}

void MinerResult::addBlock(const Block *block) {
    totalProfit += block->value;
    blocksInWinningChain++;
}
