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
#include "default_miner.hpp"
#include "publishing_strategy.hpp"
#include "strategy.hpp"

#include <cassert>

using std::placeholders::_1;
using std::placeholders::_2;

Value lazyValueInMinedChild(const Blockchain &blockchain, const Block &mineHere);
Block &lazyBlockToMineOnAtomic(const Miner &me, const Blockchain &chain);
Block &lazyBlockToMineOnNonAtomic(const Miner &me, const Blockchain &chain);

std::unique_ptr<Strategy> createLazyForkStrategy(bool atomic) {
    
    ParentSelectorFunc mineFunc;
    
    if (atomic) {
        mineFunc = lazyBlockToMineOnAtomic;
    } else {
        mineFunc = lazyBlockToMineOnNonAtomic;
    }
    auto valueFunc = lazyValueInMinedChild;
    
    return std::make_unique<Strategy>("lazy-fork", mineFunc, valueFunc);
}

Block &lazyBlockToMineOnAtomic(const Miner &me, const Blockchain &chain) {
    if (chain.getMaxHeightPub() == BlockHeight(0)) {
        return chain.most(chain.getMaxHeightPub(), me);
    }
    
    if (chain.rem(chain.most(chain.getMaxHeightPub(), me)) >= chain.gap(chain.getMaxHeightPub())) {
        return chain.most(chain.getMaxHeightPub(), me);
    } else {
        return chain.most(chain.getMaxHeightPub() - BlockHeight(1), me);
    }
}

Block &lazyBlockToMineOnNonAtomic(const Miner &, const Blockchain &chain) {
    if (chain.getMaxHeightPub() == BlockHeight(0)) {
        return chain.most(chain.getMaxHeightPub());
    }
    
    if (chain.rem(chain.most(chain.getMaxHeightPub())) >= chain.gap(chain.getMaxHeightPub())) {
        return chain.most(chain.getMaxHeightPub());
    } else {
        return chain.most(chain.getMaxHeightPub() - BlockHeight(1));
    }
}

Value lazyValueInMinedChild(const Blockchain &chain, const Block &mineHere) {
    return chain.rem(mineHere) / Value(2.0) + mineHere.nextBlockReward();
}



