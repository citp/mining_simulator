//
//  learning_strategy.hpp
//  BlockSim
//
//  Created by Harry Kalodner on 10/25/16.
//  Copyright © 2016 Harry Kalodner. All rights reserved.
//

#ifndef learning_strategy_hpp
#define learning_strategy_hpp

#include "BlockSim/strategy.hpp"
#include "BlockSim/typeDefs.hpp"

#include <stdio.h>

class LearningStrategy {
public:
    std::unique_ptr<Strategy> strat;
    StratWeight weight;
    
    LearningStrategy(std::unique_ptr<Strategy> strat, StratWeight weight);
};


#endif /* learning_strategy_hpp */
