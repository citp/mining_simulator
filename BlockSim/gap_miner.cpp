//
//  gap_miner.cpp
//  BlockSim
//
//  Created by Harry Kalodner on 5/25/16.
//  Copyright © 2016 Harry Kalodner. All rights reserved.
//

#include "gap_miner.hpp"

#include "blockchain.hpp"
#include "block.hpp"
#include "logging.h"
#include "miner.hpp"
#include "default_miner.hpp"
#include "publishing_strategy.hpp"
#include "strategy.hpp"

#include <cmath>
#include <cassert>

using std::placeholders::_1;
using std::placeholders::_2;

bool shouldMine(const Miner &me, const Blockchain &blockchain, const Block &block);

std::unique_ptr<Strategy> createGapStrategy(bool atomic, bool noiseInTransactions) {
    ParentSelectorFunc mineFunc;
    if (atomic) {
        mineFunc = defaultBlockToMineOnAtomic;
    } else {
        mineFunc = defaultBlockToMineOnNonAtomic;
    }
    
    auto valueFunc = std::bind(defaultValueInMinedChild, _1, _2, noiseInTransactions);
    
    return std::make_unique<Strategy>("gap", mineFunc, valueFunc, shouldMine);    
}

bool shouldMine(const Miner &me, const Blockchain &blockchain, const Block &block) {
    auto expectedValue = blockchain.chanceToWin(me.params.hashRate) * block.value; //chance to win * value won
    auto shouldMine = me.params.costPerSecond < expectedValue || ownBlock(&me, block.parent);
    if (!shouldMine) {
        COMMENTARY("\n Miner " << me.params.name << " declines to mine. (cost:" << me.params.costPerSecond);
        COMMENTARY(" expected payout:" << expectedValue << ")");
    }
    return shouldMine;
}
