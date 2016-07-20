//
//  clever_miner.cpp
//  BlockSim
//
//  Created by Harry Kalodner on 5/25/16.
//  Copyright © 2016 Harry Kalodner. All rights reserved.
//

#include "clever_miner.hpp"
#include "block.hpp"
#include "blockchain.hpp"
#include "logging.h"
#include "utils.hpp"
#include "miner.hpp"
#include "undercut_miner.hpp"
#include "mining_style.hpp"
#include "strategy.hpp"

#include <iostream>
#include <cassert>

using std::placeholders::_1;
using std::placeholders::_2;

Value valueInMinedChild(const Blockchain &blockchain, const Block &mineHere);

Strategy createCleverForkStrategy(bool noSelfMining) {
    auto mineFunc = std::bind(undercutblockToMineOn, _1, _2, noSelfMining);
    
    auto defaultCreator = [=](MinerParameters params, const Strategy &strat) { return std::make_unique<Miner>(params, strat, mineFunc, valueInMinedChild); };
    
    return {"undercut-fork", defaultCreator};
}

Value valueInMinedChild(const Blockchain &blockchain, const Block &mineHere)  {
    //Value valueMin = B;
    auto moneyLeftInNetwork = calculateMoneyLeftInNetwork(blockchain.getValueNetworkTotal(), mineHere);
    return moneyLeftInNetwork / Value(2.0) + mineHere.nextBlockReward;
}


bool CleverPublishingStyle::publishTest(const Blockchain &blockchain, MinedBlock &head, const Miner &me) const {
    if (head.value < calculateMoneyLeftInNetwork(blockchain.getValueNetworkTotal(), head) + head.nextBlockReward) {
        //publish your block
        return true;
    } else if (ownBlock(me, head.parent)) {
        // publish your block (to protect your past block)
        COMMENTARY("Miner " << me.params.name << " publishes " << head << " to protect past block." << std::endl);
        return true;
    }
    return false;
}
