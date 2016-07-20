//
//  simple_miner.cpp
//  BlockSim
//
//  Created by Harry Kalodner on 6/11/16.
//  Copyright © 2016 Harry Kalodner. All rights reserved.
//

#include "simple_mining_style.hpp"
#include "blockchain.hpp"
#include "utils.hpp"
#include "block.hpp"

#include <assert.h>
#include <cmath>

std::unique_ptr<MinedBlock> SimpleMiningStyle::attemptToMineImp(const Blockchain &blockchain, Miner &me) {
    assert(_hasInitializedTime);
    
    auto block = createBlock(blockchain, me);
    
    _nextBlockTime = generateNewMiningTime(blockchain, me);
    
    return block;
}

void SimpleMiningStyle::initialize(const Blockchain &blockchain, const Miner &miner) {
    _nextBlockTime = generateNewMiningTime(blockchain, miner);
    _hasInitializedTime = true;
}

BlockTime SimpleMiningStyle::nextMiningTime() const {
    assert(_hasInitializedTime);
    return _nextBlockTime;
}

Value SimpleMiningStyle::moneySpentMining(const Miner &miner) const {
    return miner.params.costPerSecond * getTimeReached();
}

BlockTime SimpleMiningStyle::generateNewMiningTime(const Blockchain &blockchain, const Miner &miner) {
    return getTimeReached() + BlockTime(1) + selectMiningOffset(blockchain.chanceToWin(miner.params.hashRate));
}
