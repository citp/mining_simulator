//
//  undercut_miner.cpp
//  BlockSim
//
//  Created by Harry Kalodner on 5/25/16.
//  Copyright © 2016 Harry Kalodner. All rights reserved.
//

#include "undercut_miner.hpp"

#include "block.hpp"
#include "blockchain.hpp"
#include "miner.hpp"
#include "logging.h"
#include "utils.hpp"
#include "default_miner.hpp"
#include "publishing_strategy.hpp"
#include "mining_style.hpp"
#include "strategy.hpp"

#include <iostream>
#include <cassert>

using std::placeholders::_1;
using std::placeholders::_2;

Value valueInMinedChild(const Blockchain &blockchain, const Block &mineHere, bool noiseInTransactions);

Strategy createUndercutStrategy(bool noSelfMining, bool noiseInTransactions) {
    auto mineFunc = std::bind(undercutblockToMineOn, _1, _2, noSelfMining);
    auto valueFunc = std::bind(valueInMinedChild, _1, _2, noiseInTransactions);
    
    auto defaultCreator = [=](MinerParameters params, const Strategy &strat) { return std::make_unique<Miner>(params, strat, mineFunc, valueFunc); };
    
    return {"undercut-fork", defaultCreator};
}

Block &undercutblockToMineOn(const Miner &me, const Blockchain &blockchain, bool noSelfMining) {
    //you want to mine only on blocks leaving the most money in the network
    //or make a new block
    auto block = me.getLastMinedBlock();
    if (!noSelfMining && block && block->get().height >= blockchain.getMaxHeightPub()) {
        return *block;
    } else {
        //out of the published heads at max height, you mine on the one with the max val left in network
        Block &mineHere = blockchain.smallestPublishedHead(BlockHeight(0));
        Value moneyLeftInNetwork = calculateMoneyLeftInNetwork(blockchain.getValueNetworkTotal(), mineHere);
        //if the best node to mine off of is not public, or there is less money in the network than in the block
        //you should try to fork
        if (mineHere.value >= moneyLeftInNetwork + mineHere.nextBlockReward && mineHere.height > BlockHeight(0)) {
            return blockchain.smallestPublishedHead(BlockHeight(1));
        } else {
            return mineHere;
        }
    }
    
}

Value valueInMinedChild(const Blockchain &blockchain, const Block &mineHere, bool noiseInTransactions) {
    // forking
    if (mineHere.height == blockchain.getMaxHeightPub() - BlockHeight(1)) {
        return mineHere.value - UNDERCUT_VALUE;
    } else {
        return defaultValueInMinedChild(blockchain, mineHere, noiseInTransactions);
    }
}

