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
#include "minerImp.hpp"
#include "mining_style.hpp"

#include "simple_publisher.hpp"
#include "picky_mining_style.hpp"
#include "simple_mining_style.hpp"

#include <assert.h>
#include <iostream>

Strategy::Strategy(std::string name_, MinerImpCreator minerImpCreator_) : minerImpCreator(minerImpCreator_), name(name_) {}

std::ostream& operator<<(std::ostream& os, const Strategy& strat) {
    os << strat.name;
    return os;
}

std::unique_ptr<MinerImp> Strategy::createImp() const {
    return minerImpCreator();
}
