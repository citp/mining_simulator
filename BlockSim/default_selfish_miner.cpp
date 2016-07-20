//
//  default_selfish_miner.cpp
//  BlockSim
//
//  Created by Harry Kalodner on 6/6/16.
//  Copyright © 2016 Harry Kalodner. All rights reserved.
//

#include "default_selfish_miner.hpp"
#include "block.hpp"
#include "blockchain.hpp"
#include "miner.hpp"
#include "selfish_miner.hpp"
#include "default_miner.hpp"
#include "logging.h"
#include "utils.hpp"
#include "mining_style.hpp"
#include "strategy.hpp"

#include <iostream>

using std::placeholders::_1;
using std::placeholders::_2;

Block &blockToMineOn(const Miner &me, const Blockchain &blockchain, double gamma);

Strategy createDefaultSelfishStrategy(bool noiseInTransactions, double gamma) {
    auto mineFunc = std::bind(blockToMineOn, _1, _2, gamma);
    auto valueFunc = std::bind(defaultValueInMinedChild, _1, _2, noiseInTransactions);
    
    auto defaultCreator = [=](MinerParameters params, const Strategy &strat) { return std::make_unique<Miner>(params, strat, mineFunc, valueFunc); };
    
    return {"default-selfish", defaultCreator};
}

Block &blockToMineOn(const Miner &me, const Blockchain &blockchain, double gamma) {
    
    std::vector<Block*> possiblities = blockchain.oldestPublishedHeads();
    if (possiblities.size() == 1) { //no forking
        return *possiblities[0];
    }
    else if (possiblities.size() == 2) { //fork between the selfish miner and the rest of the network
        //mineHere should already be set to the side of the fork not the selfish miner
        Block *selfishBlock = nullptr;
        Block *defaultBlock = nullptr;
        if (ownBlock(me, *possiblities[0])) {
            defaultBlock = possiblities[0];
            selfishBlock = possiblities[1];
        } else {
            defaultBlock = possiblities[1];
            selfishBlock = possiblities[0];
        }
        
        if (!ownBlock(me, *selfishBlock)) {
            ERROR("\n####ERROR IN SELFISH MINING SIM LOGIC###\n\n" << std::endl);
            return *defaultBlock;
        } else {
            //with chance gamma, mine on the selfish miner's block, otherwise not
            if (selectRandomChance() < gamma) {
                return *selfishBlock;
                COMMENTARY("Having to mine on selfish block due to gamma. ");
            } else {
                return *defaultBlock;
            }
        }
    } else { //lolwut
        ERROR("\n#####ERROR UNFORSEEN CIRCUMSTANCES IN LOGIC FOR SELFISH MINING SIM###\n\n" << std::endl);
        return *possiblities[0];
    }
}
