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
#include "minerImp.hpp"

#include <iostream>
#include <cassert>

using std::placeholders::_1;
using std::placeholders::_2;

Block &blockToMineOnAtomic(const Miner &me, const Blockchain &chain);
Block &blockToMineOnNonAtomic(const Miner &me, const Blockchain &chain);

Strategy createPettyStrategy(bool atomic, bool noiseInTransactions) {

    ParentSelectorFunc mineFunc;
    
    if (atomic) {
        mineFunc = blockToMineOnAtomic;
    } else {
        mineFunc = blockToMineOnNonAtomic;
    }
    auto valueFunc = std::bind(defaultValueInMinedChild, _1, _2, noiseInTransactions);
    
    auto impCreator = [=]() {
        return std::make_unique<MinerImp>(mineFunc, valueFunc);
    };
    
    return {"petty-honest", impCreator};
}

Block &blockToMineOnAtomic(const Miner &me, const Blockchain &chain) {
    return chain.most(BlockHeight(0), me);
}

Block &blockToMineOnNonAtomic(const Miner &, const Blockchain &chain) {
    return chain.smallestHead(BlockHeight(0));
}
