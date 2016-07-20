//
//  strategy.cpp
//  BlockSim
//
//  Created by Harry Kalodner on 5/31/16.
//  Copyright © 2016 Harry Kalodner. All rights reserved.
//

#include "strategy.hpp"
#include "minerStrategies.h"
#include "block.hpp"
#include "miner.hpp"
#include "mining_style.hpp"

#include <assert.h>
#include <iostream>

Strategy::Strategy(std::string name_, MinerCreatePtr creator_) : name(name_), creator(creator_) {}

std::ostream& operator<<(std::ostream& os, const Strategy& strat) {
    os << strat.name;
    return os;
}

std::unique_ptr<Miner> Strategy::generateMiner(MinerParameters params) const {
    return creator(params, *this);
}
