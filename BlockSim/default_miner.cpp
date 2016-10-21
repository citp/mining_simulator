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
#include "logging.h"
#include "utils.hpp"
#include "miner.hpp"
#include "publishing_strategy.hpp"
#include "mining_style.hpp"
#include "strategy.hpp"
#include "minerImp.hpp"

#include <iostream>
#include <cmath>
#include <cassert>

using std::placeholders::_1;
using std::placeholders::_2;

Strategy createDefaultStrategy(bool noSelfMining, bool noiseInTransactions) {
    auto mineFunc = std::bind(defaultBlockToMineOn, _1, _2, noSelfMining);
    auto valueFunc = std::bind(defaultValueInMinedChild, _1, _2, noiseInTransactions);
    
    auto impCreator = [=]() {
        return std::make_unique<MinerImp>(mineFunc, valueFunc);
    };
    
    return {"default-honest", impCreator};
}

Block &defaultBlockToMineOn(const Miner &me, const Blockchain &blockchain, bool noSelfMining) {
    auto block = me.getLastMinedBlock();
    if (!noSelfMining && block && block->get().height >= blockchain.getMaxHeightPub()) {
        return *block;
    } else {
        return blockchain.oldestPublishedHead();
    }
}

Value defaultValueInMinedChild(const Blockchain &blockchain, const Block &mineHere, bool noiseInTransactions) {
    auto minVal = mineHere.nextBlockReward;
    auto maxVal = getRem(blockchain.getTotalFees(), mineHere) + mineHere.nextBlockReward;
    //this represents some noise-- no noise, value would = valueMax
    //value = ((valueMax - valueMin)*((dis(gen)+.7)/1.7)) + valueMin;
    auto value = maxVal;
    if (noiseInTransactions) {
        value = valWithNoise(minVal, maxVal);
    }
    return value;
}
