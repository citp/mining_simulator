//
//  gap_miner.cpp
//  BlockSim
//
//  Created by Harry Kalodner on 5/25/16.
//  Copyright © 2016 Harry Kalodner. All rights reserved.
//

#include "gap_miner.hpp"

#include "utils.hpp"
#include "blockchain.hpp"
#include "block.hpp"
#include "logging.h"
#include "miner.hpp"
#include "default_miner.hpp"
#include "publishing_strategy.hpp"
#include "mining_style.hpp"
#include "strategy.hpp"

#include <cmath>
#include <iostream>
#include <cassert>

using std::placeholders::_1;
using std::placeholders::_2;

bool shouldMine(const Miner &me, const Blockchain &blockchain, const MinedBlock &block);

Strategy createGapStrategy(bool noSelfMining, bool noiseInTransactions) {
    auto mineFunc = std::bind(defaultBlockToMineOn, _1, _2, noSelfMining);
    auto valueFunc = std::bind(defaultValueInMinedChild, _1, _2, noiseInTransactions);
    
    auto defaultCreator = [=](MinerParameters params, const Strategy &strat) { return std::make_unique<Miner>(params, strat, mineFunc, valueFunc, shouldMine); };
    
    return {"gap", defaultCreator};
}

bool shouldMine(const Miner &me, const Blockchain &blockchain, const MinedBlock &block) {
    auto expectedValue = blockchain.chanceToWin(me.params.hashRate) * block.value; //chance to win * value won
    auto shouldMine = me.params.costPerSecond < expectedValue || ownBlock(me, block.parent);
    if (!shouldMine) {
        COMMENTARY("\n Miner " << me.params.name << " declines to mine. (cost:" << me.params.costPerSecond);
        COMMENTARY(" expected payout:" << expectedValue << ")");
    }
    return shouldMine;
}
