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
Block &lazyBlockToMineOnAtomic(const Miner &me, const Blockchain &chain);
Block &lazyBlockToMineOnNonAtomic(const Miner &me, const Blockchain &chain);

Strategy createLazyForkStrategy(bool atomic) {
    
    ParentSelectorFunc mineFunc;
    
    if (atomic) {
        mineFunc = lazyBlockToMineOnAtomic;
    } else {
        mineFunc = lazyBlockToMineOnNonAtomic;
    }
    auto valueFunc = lazyValueInMinedChild;
    
    auto impCreator = [=]() {
        return std::make_unique<MinerImp>(mineFunc, valueFunc);
    };
    
    return {"lazy-fork", impCreator};
}

Block &lazyBlockToMineOnAtomic(const Miner &me, const Blockchain &chain) {
    if (chain.getMaxHeightPub() == BlockHeight(0) || chain.rem(chain.most(BlockHeight(0), me)) > chain.gap(BlockHeight(0))) {
        return chain.most(BlockHeight(0), me);
    } else {
        return chain.most(BlockHeight(1), me);
    }
}

Block &lazyBlockToMineOnNonAtomic(const Miner &, const Blockchain &chain) {
    if (chain.getMaxHeightPub() == BlockHeight(0) || chain.rem(chain.smallestHead(BlockHeight(0))) > chain.gap(BlockHeight(0))) {
        return chain.smallestHead(BlockHeight(0));
    } else {
        return chain.smallestHead(BlockHeight(1));
    }
}

Value lazyValueInMinedChild(const Blockchain &chain, const Block &mineHere) {
    return chain.rem(mineHere) / Value(2.0) + mineHere.nextBlockReward();
}



