//
//  function_fork_miner.cpp
//  BlockSim
//
//  Created by Harry Kalodner on 5/25/16.
//  Copyright © 2016 Harry Kalodner. All rights reserved.
//

#include "function_fork_miner.hpp"
#include "block.hpp"
#include "blockchain.hpp"
#include "logging.h"
#include "utils.hpp"
#include "miner.hpp"
#include "publishing_strategy.hpp"
#include "mining_style.hpp"
#include "strategy.hpp"

#include <gsl/gsl_sf_lambert.h>

#include <iostream>
#include <cmath>
#include <cassert>

using std::placeholders::_1;
using std::placeholders::_2;

Block &blockToMineOn(const Miner &me, const Blockchain &blockchain, bool noSelfMining, FunctionForkFunc functionForkFunc);
Value valueInMinedChild(const Blockchain &blockchain, const Block &block, FunctionForkFunc functionForkFunc);

Strategy createFunctionForkStrategy(bool noSelfMining, FunctionForkFunc functionForkFunc, std::string type) {
    auto mineFunc = std::bind(blockToMineOn, _1, _2, noSelfMining, functionForkFunc);
    auto valueFunc = std::bind(valueInMinedChild, _1, _2, functionForkFunc);
    
    auto defaultCreator = [=](MinerParameters params, const Strategy &strat) { return std::make_unique<Miner>(params, strat, mineFunc, valueFunc); };
    
    return {"function-fork-" + type, defaultCreator};
}

Value valueInMinedChild(const Blockchain &blockchain, const Block &block, FunctionForkFunc functionForkFunc) {
    auto moneyLeftInNetwork = calculateMoneyLeftInNetwork(blockchain.getValueNetworkTotal(), block);
    auto value = functionForkFunc(blockchain, moneyLeftInNetwork) + block.nextBlockReward;
    
    auto childWithMinValue = block.smallestChild();
    if (childWithMinValue != nullptr && value >= childWithMinValue->value) {
        value = childWithMinValue->value - UNDERCUT_VALUE; //make sure to undercut if needbe
    }
    //    Value valueMax = moneyLeftInNetwork + block.nextBlockReward;
    return value;
}

Block &blockToMineOn(const Miner &me, const Blockchain &blockchain, bool noSelfMining, FunctionForkFunc functionForkFunc) {
    auto block = me.getLastMinedBlock();
    if (!noSelfMining && block && block->get().height >= blockchain.getMaxHeightPub()) {
        return *block;
    } else {
        auto &mineHere = blockchain.smallestPublishedHead(BlockHeight(0));
        
        if (mineHere.height == BlockHeight(0)) {
            return mineHere;
        }
        
        auto &mineHereFork = blockchain.smallestPublishedHead(BlockHeight(1));
        auto *childWithMinValue = mineHereFork.smallestChild();
        
        auto valueHere = valueInMinedChild(blockchain, mineHere, functionForkFunc);
        auto valueFork = valueInMinedChild(blockchain, mineHereFork, functionForkFunc);
        
        COMMENTARY("value from continuing:" << valueHere << " ");
        COMMENTARY("value from forking:" << valueFork << " ");
        
        //At this point, you have 2 options, mineHere and mineHereFork. mineHere gives you the best place to mine,
        //continuing the chain. mineHereFork gives the best place to fork (by most value left in network)
        //moneyLeftInNetworkParent is going to be always bigger. But, if you fork, you must undercut-- so make
        //that decision here:
        
        if(childWithMinValue == nullptr || valueFork < valueHere) {  //fork in tie vs. continue in tie
            COMMENTARY("Continuing. ");
            return mineHere;
        }
        else {
            COMMENTARY("Forking. ");
            return mineHereFork;
        }
    }
}

Value functionForkPercentage(const Blockchain &, Value maxVal, double funcCoeff) {
    return maxVal / Value(funcCoeff);
}

Value functionForkLambert(const Blockchain &blockchain, Value maxVal, double lambertCoeff) {
    //don't include B-- this is about the expected portion form tx fees
    auto expectedBlockSize = blockchain.expectedBlockSize();
    auto maxValRaw = rawValue(maxVal);
    auto expectedSizeRaw = rawValue(expectedBlockSize);
    if (maxValRaw / expectedSizeRaw <= lambertCoeff) {
        return maxVal;
    } else if (maxValRaw / expectedSizeRaw < 2*lambertCoeff - std::log(lambertCoeff) - 1) {
        
        double argToLambertFunct0 = -lambertCoeff*std::exp((maxValRaw/expectedSizeRaw)-2*lambertCoeff);
        double lambertRes = gsl_sf_lambert_W0(argToLambertFunct0);
        return Value(-lambertRes*expectedSizeRaw);
    } else {
        return Value(expectedBlockSize);
    }
}
