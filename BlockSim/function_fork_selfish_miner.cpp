//
//  function_fork_selfish_miner.cpp
//  BlockSim
//
//  Created by Harry Kalodner on 11/4/16.
//  Copyright © 2016 Harry Kalodner. All rights reserved.
//

#include "function_fork_selfish_miner.hpp"
#include "clever_selfish_miner.hpp"
#include "strategy.hpp"
#include "block.hpp"

using std::placeholders::_1;
using std::placeholders::_2;

std::unique_ptr<Strategy> createFunctionForkSelfishStrategy(Value cutoff, ForkFunc func) {
    auto valueFunc = std::bind(functionForkValueInMinedChild, _1, _2, func);
    
    return std::make_unique<Strategy>("function-selfish", selfishBlockToMineOn, valueFunc, std::make_unique<CleverSelfishPublishingStyle>(cutoff));
}
