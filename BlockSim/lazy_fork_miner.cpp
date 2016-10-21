//
//  lazy_miner.cpp
//  BlockSim
//
//  Created by Harry Kalodner on 5/25/16.
//  Copyright © 2016 Harry Kalodner. All rights reserved.
//

#include "lazy_fork_miner.hpp"

#include "block.hpp"
#include "blockchain.hpp"
#include "miner.hpp"
#include "logging.h"
#include "utils.hpp"
#include "default_miner.hpp"
#include "publishing_strategy.hpp"
#include "mining_style.hpp"
#include "strategy.hpp"
#include "minerImp.hpp"

#include <iostream>
#include <cassert>

using std::placeholders::_1;
using std::placeholders::_2;

Value lazyValueInMinedChild(const Blockchain &blockchain, const Block &mineHere);
Block &lazyBlockToMineOn(const Miner &me, const Blockchain &blockchain, bool noSelfMining);

Strategy createLazyForkStrategy(bool noSelfMining) {
    auto mineFunc = std::bind(lazyBlockToMineOn, _1, _2, noSelfMining);
    auto valueFunc = std::bind(lazyValueInMinedChild, _1, _2);
    
    auto impCreator = [=]() {
        return std::make_unique<MinerImp>(mineFunc, valueFunc);
    };
    
    return {"lazy-fork", impCreator};
}

Block &lazyBlockToMineOn(const Miner &me, const Blockchain &blockchain, bool noSelfMining) {
    //you want to mine only on blocks leaving the most money in the network
    //or make a new block
    auto block = me.getLastMinedBlock();
    if (!noSelfMining && block && block->get().height >= blockchain.getMaxHeightPub()) {
        return *block;
    } else {
        //out of the published heads at max height, you mine on the one with the max val left in network
        Block &mineHere = blockchain.smallestHead(BlockHeight(0));
        Value moneyLeftInNetwork = getRem(blockchain.getTotalFees(), mineHere);
        //if the best node to mine off of is not public, or there is less money in the network than in the block
        //you should try to fork
        if (mineHere.value >= moneyLeftInNetwork + mineHere.nextBlockReward && mineHere.height > BlockHeight(0)) {
            return blockchain.smallestHead(BlockHeight(1));
        } else {
            return mineHere;
        }
    }
    
}

Value lazyValueInMinedChild(const Blockchain &blockchain, const Block &mineHere) {
    auto feePool = getRem(blockchain.getTotalFees(), mineHere);
    return feePool / Value(2.0) + mineHere.nextBlockReward;
}

Block &blockToMineOn(const Miner &me, const Blockchain &blockchain);
Value blockValue(const Blockchain &blockchain, const Block &mineHere);


Block &blockToMineOn(const Miner &me, const Blockchain &blockchain) {
    auto block = me.getLastMinedBlock();
    if (!block && block->get().height >= blockchain.getMaxHeightPub()) {
        return *block;
    } else {
        Block &smallestHead = blockchain.smallestHead(BlockHeight(0));
        Value rem = getRem(blockchain.getTotalFees(), smallestHead);
        if (smallestHead.value >= rem + smallestHead.nextBlockReward &&
            smallestHead.height > BlockHeight(0)) {
            return blockchain.smallestHead(BlockHeight(1));
        } else {
            return smallestHead;
        }
    }
    
}

Value blockValue(const Blockchain &blockchain, const Block &mineHere) {
    auto rem = getRem(blockchain.getTotalFees(), mineHere);
    return rem / Value(2.0) + mineHere.nextBlockReward;
}



