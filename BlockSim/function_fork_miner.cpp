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
#include "minerImp.hpp"

#include <gsl/gsl_sf_lambert.h>

#include <iostream>
#include <cmath>
#include <cassert>

using std::placeholders::_1;
using std::placeholders::_2;

Block &blockToMineOnNonAtomic(const Miner &, const Blockchain &chain, ForkFunc f);
Block &blockToMineOnAtomic(const Miner &me, const Blockchain &chain, ForkFunc f);

Value valueInMinedChild(const Blockchain &blockchain, const Block &block, ForkFunc f);

Value valCont(const Blockchain &chain, ForkFunc f);
Value valUnder(const Blockchain &chain, ForkFunc f);

Strategy createFunctionForkStrategy(bool atomic, ForkFunc f, std::string type) {
    ParentSelectorFunc mineFunc;
    
    if (atomic) {
        mineFunc = std::bind(blockToMineOnAtomic, _1, _2, f);
    } else {
        mineFunc = std::bind(blockToMineOnNonAtomic, _1, _2, f);
    }
    auto valueFunc = std::bind(valueInMinedChild, _1, _2, f);
    
    
    auto impCreator = [=]() {
        return std::make_unique<MinerImp>(mineFunc, valueFunc);
    };
    
    return {"function-fork-" + type, impCreator};
}

Value valCont(const Blockchain &chain, ForkFunc f) {
    auto &block = chain.smallestHead(BlockHeight(0));
    return f(chain, chain.rem(block)) + block.nextBlockReward();
}

Value valUnder(const Blockchain &chain, ForkFunc f) {
    auto &block = chain.smallestHead(BlockHeight(1));
    return std::min(f(chain, chain.rem(block)), chain.gap(BlockHeight(0)) - UNDERCUT_VALUE) + block.nextBlockReward();
}

Block &blockToMineOnNonAtomic(const Miner &, const Blockchain &chain, ForkFunc f) {
    if (chain.getMaxHeightPub() == BlockHeight(0) || valCont(chain, f) > valUnder(chain, f)) {
        return chain.smallestHead(BlockHeight(0));
    } else {
        return chain.smallestHead(BlockHeight(1));
    }
}

Block &blockToMineOnAtomic(const Miner &me, const Blockchain &chain, ForkFunc f) {
    if (chain.getMaxHeightPub() == BlockHeight(0) ||
        chain.most(BlockHeight(0), me).minedBy(me) ||
        valCont(chain, f) > valUnder(chain, f)) {
        return chain.most(BlockHeight(0), me);
    } else {
        return chain.most(BlockHeight(1), me);
    }
}

Value valueInMinedChild(const Blockchain &chain, const Block &block, ForkFunc f) {
    if (block.isHead()) {
        return valCont(chain, f);
    } else {
        return valUnder(chain, f);
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
