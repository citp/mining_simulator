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

PickyMiningStyle::PickyMiningStyle(ParentSelectorFunc parentSelectorFunc_, BlockValueFunc blockValueFunc_, ShouldMineFunc shouldMineFunc_) : MiningStyle(parentSelectorFunc_, blockValueFunc_), shouldMineFunc(shouldMineFunc_)  {}

BlockTime PickyMiningStyle::nextMiningTime(const Blockchain &chain, const Miner &) const {
    return chain.getTime() + BlockTime(1);
}

std::pair<std::unique_ptr<Block>, Value> PickyMiningStyle::attemptToMine(Blockchain &blockchain, Miner *miner, BlockTime) {
    auto block = createBlock(blockchain, *miner);
    if (shouldMineFunc(*miner, blockchain, *block)) {
        Value cost = miner->params.costPerSecond * BlockTime(1);
        if (TimeRate(selectRandomChance()) < blockchain.chanceToWin(miner->params.hashRate)) {
            return std::make_pair(std::move(block), cost);
        } else {
            return std::make_pair(nullptr, cost);
        }
    }
    
    return std::make_pair(nullptr, Value(0));
}

Value PickyMiningStyle::resetMiningCost(const Miner &, const Blockchain &, BlockTime) {
    return Value(0);
}
