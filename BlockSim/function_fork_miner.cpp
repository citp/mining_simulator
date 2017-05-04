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
#include "miner.hpp"
#include "publishing_strategy.hpp"
#include "strategy.hpp"

#include <gsl/gsl_sf_lambert.h>

#include <cmath>
#include <cassert>

#include <iostream>

using std::placeholders::_1;
using std::placeholders::_2;

Block &blockToMineOnNonAtomic(const Miner &, const Blockchain &chain, ForkFunc f);
Block &blockToMineOnAtomic(const Miner &me, const Blockchain &chain, ForkFunc f);

Value valCont(const Blockchain &chain, ForkFunc f, const Block &block);
Value valUnder(const Blockchain &chain, ForkFunc f, const Block &block);

std::unique_ptr<Strategy> createFunctionForkStrategy(bool atomic, ForkFunc f, std::string type) {
    ParentSelectorFunc mineFunc;
    
    if (atomic) {
        mineFunc = std::bind(blockToMineOnAtomic, _1, _2, f);
    } else {
        mineFunc = std::bind(blockToMineOnNonAtomic, _1, _2, f);
    }
    auto valueFunc = std::bind(functionForkValueInMinedChild, _1, _2, f);
    
    return std::make_unique<Strategy>("function-fork-" + type, mineFunc, valueFunc);
}

Block &blockToMineOnNonAtomic(const Miner &, const Blockchain &chain, ForkFunc f) {
    Block &contBlock = chain.most(chain.getMaxHeightPub());
    if (chain.getMaxHeightPub() == BlockHeight(0)) {
        return contBlock;
    }
    Block &underBlock = chain.most(chain.getMaxHeightPub() - BlockHeight(1));
    
    if (valCont(chain, f, contBlock) >= valUnder(chain, f, underBlock)) {
        return contBlock;
    } else {
        return underBlock;
    }
}

Block &blockToMineOnAtomic(const Miner &me, const Blockchain &chain, ForkFunc f) {
    Block &contBlock = chain.most(chain.getMaxHeightPub(), me);
    if (chain.getMaxHeightPub() == BlockHeight(0)) {
        return contBlock;
    }
    
    Block &underBlock = chain.most(chain.getMaxHeightPub() - BlockHeight(1), me);
    if (contBlock.minedBy(&me) || valCont(chain, f, contBlock) >= valUnder(chain, f, underBlock)) {
        return contBlock;
    } else {
        return underBlock;
    }
}

Value functionForkValueInMinedChild(const Blockchain &chain, const Block &block, ForkFunc f) {
    if (block.height == chain.getMaxHeightPub()) {
        return valCont(chain, f, block);
    } else {
        return valUnder(chain, f, block);
    }
}

Value valCont(const Blockchain &chain, ForkFunc f, const Block &contBlock) {
    return f(chain, chain.rem(contBlock)) + contBlock.nextBlockReward();
}

Value valUnder(const Blockchain &chain, ForkFunc f, const Block &underBlock) {
    return std::min(f(chain, chain.rem(underBlock)), chain.gap(chain.getMaxHeightPub()) - UNDERCUT_VALUE) + underBlock.nextBlockReward();
}

Value functionForkPercentage(const Blockchain &, Value maxVal, double funcCoeff) {
    double coeff = 1.0 / funcCoeff;
    double newValue = rawValue(maxVal) * coeff;
    return Value(static_cast<ValueType>(newValue));
}

Value functionForkLambert(const Blockchain &blockchain, Value maxVal, double lambertCoeff) {
    //don't include B-- this is about the expected portion form tx fees
    auto expectedBlockSize = blockchain.expectedBlockSize();
    auto expectedSizeRaw = rawValue(expectedBlockSize);
    auto blockRatio = valuePercentage(maxVal, Value(expectedBlockSize));
    if (blockRatio <= lambertCoeff) {
        return maxVal;
    } else if (blockRatio < 2*lambertCoeff - std::log(lambertCoeff) - 1) {
        
        double argToLambertFunct0 = -lambertCoeff*std::exp(blockRatio-2*lambertCoeff);
        double lambertRes = gsl_sf_lambert_W0(argToLambertFunct0);
        
        double newValue = -expectedSizeRaw * lambertRes;
        return Value(static_cast<ValueType>(newValue));
    } else {
        return Value(expectedBlockSize);
    }
}
