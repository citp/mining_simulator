//
//  picky_miner.cpp
//  BlockSim
//
//  Created by Harry Kalodner on 6/12/16.
//  Copyright © 2016 Harry Kalodner. All rights reserved.
//

#include "picky_mining_style.hpp"
#include "utils.hpp"
#include "blockchain.hpp"
#include "block.hpp"
#include "miner.hpp"

#include <assert.h>
#include <cmath>

void PickyMiningStyle::initialize(const Blockchain &blockchain, const Miner &miner) {
    MiningStyle::initialize(blockchain, miner);
    totalMiningCost = Value(0);
}

Value PickyMiningStyle::moneySpentMining(const Miner &) const {
    return totalMiningCost;
}

BlockTime PickyMiningStyle::nextMiningTime() const {
    return getTimeReached() + BlockTime(1);
}

std::unique_ptr<MinedBlock> PickyMiningStyle::attemptToMineImp(const Blockchain &blockchain, Miner &miner) {
    if (TimeRate(selectRandomChance()) < blockchain.chanceToWin(miner.params.hashRate)) {
        auto block = createBlock(blockchain, miner);
        if (shouldMineFunc(miner, blockchain, *block)) {
            totalMiningCost += miner.params.costPerSecond * BlockTime(1);
            return block;
        }
    }
    
    return nullptr;
}
