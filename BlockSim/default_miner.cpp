//
//  default_miner.cpp
//  BlockSim
//
//  Created by Harry Kalodner on 5/25/16.
//  Copyright © 2016 Harry Kalodner. All rights reserved.
//

#include "default_miner.hpp"
#include "block.hpp"
#include "blockchain.hpp"
#include "utils.hpp"
#include "miner.hpp"
#include "publishing_strategy.hpp"
#include "strategy.hpp"

#include <cmath>
#include <cassert>

using std::placeholders::_1;
using std::placeholders::_2;

std::unique_ptr<Strategy> createDefaultStrategy(bool atomic, bool noiseInTransactions) {
    ParentSelectorFunc mineFunc;
    
    if (atomic) {
        mineFunc = defaultBlockToMineOnAtomic;
    } else {
        mineFunc = defaultBlockToMineOnNonAtomic;
    }
    
    auto valueFunc = std::bind(defaultValueInMinedChild, _1, _2, noiseInTransactions);
    
    return std::make_unique<Strategy>("default-honest", mineFunc, valueFunc);
}

Block &defaultBlockToMineOnAtomic(const Miner &me, const Blockchain &chain) {
    return chain.oldest(chain.getMaxHeightPub(), me);
}

Block &defaultBlockToMineOnNonAtomic(const Miner &, const Blockchain &chain) {
    return chain.oldestPublishedHead(chain.getMaxHeightPub());
}

Value defaultValueInMinedChild(const Blockchain &chain, const Block &mineHere, bool noiseInTransactions) {
    auto minVal = mineHere.nextBlockReward();
    auto maxVal = chain.rem(mineHere) + mineHere.nextBlockReward();
    //this represents some noise-- no noise, value would = valueMax
    //value = ((valueMax - valueMin)*((dis(gen)+.7)/1.7)) + valueMin;
    auto value = maxVal;
    if (noiseInTransactions) {
        value = valWithNoise(minVal, maxVal);
    }
    return value;
}
