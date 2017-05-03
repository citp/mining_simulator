//
//  utils.hpp
//  BlockSim
//
//  Created by Harry Kalodner on 5/25/16.
//  Copyright © 2016 Harry Kalodner. All rights reserved.
//

#ifndef utils_hpp
#define utils_hpp

#include "typeDefs.hpp"

#include <cstddef>

std::size_t selectRandomIndex(size_t size);
double selectRandomChance();
BlockTime selectMiningOffset(TimeRate mean);

Value valWithNoise(Value minVal, Value maxVal);

#endif /* utils_hpp */
