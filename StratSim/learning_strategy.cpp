//
//  learning_strategy.cpp
//  BlockSim
//
//  Created by Harry Kalodner on 10/25/16.
//  Copyright © 2016 Harry Kalodner. All rights reserved.
//

#include "learning_strategy.hpp"


LearningStrategy::LearningStrategy(std::unique_ptr<Strategy> strat_, StratWeight weight_) : strat(std::move(strat_)), weight(weight_) {}
