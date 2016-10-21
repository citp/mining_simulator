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
#include <vector>

class Block;

std::size_t selectRandomIndex(std::size_t size);
std::size_t selectWeightedIndex(const std::vector<double> &weights);
double selectRandomChance();
BlockTime selectMiningOffset(TimeRate mean);

Value valWithNoise(Value minVal, Value maxVal);

Value getRem(Value valueNetworkTotal, const Block &block);
Block *largestBlock(const std::vector<Block *> &blocks);

bool valueEquals(Value a, Value b, double epsilon = std::numeric_limits<double>::epsilon());

#endif /* utils_hpp */
