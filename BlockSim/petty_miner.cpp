//
//  petty_miner.cpp
//  BlockSim
//
//  Created by Harry Kalodner on 5/25/16.
//  Copyright © 2016 Harry Kalodner. All rights reserved.
//

#include "petty_miner.hpp"
#include "block.hpp"
#include "blockchain.hpp"
#include "logging.h"
#include "utils.hpp"
#include "miner.hpp"
#include "default_miner.hpp"
#include "publishing_strategy.hpp"
#include "mining_style.hpp"
#include "strategy.hpp"

#include <iostream>
#include <cassert>

using std::placeholders::_1;
using std::placeholders::_2;

Block &blockToMineOn(const Miner &me, const Blockchain &blockchain, bool noSelfMining);

Strategy createPettyStrategy(bool noSelfMining, bool noiseInTransactions) {
    auto mineFunc = std::bind(blockToMineOn, _1, _2, noSelfMining);
    auto valueFunc = std::bind(defaultValueInMinedChild, _1, _2, noiseInTransactions);
    
    auto defaultCreator = [=](MinerParameters params, const Strategy &strat) { return std::make_unique<Miner>(params, strat, mineFunc, valueFunc); };
    
    return {"petty-honest", defaultCreator};
}

Block &blockToMineOn(const Miner &me, const Blockchain &blockchain, bool noSelfMining) {
    auto block = me.getLastMinedBlock();
    if (!noSelfMining && block && block->get().height >= blockchain.getMaxHeightPub()) {
        return *block;
    } else {
        return blockchain.smallestPublishedHead(BlockHeight(0));
    }
}
