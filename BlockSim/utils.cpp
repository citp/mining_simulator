//
//  utils.cpp
//  BlockSim
//
//  Created by Harry Kalodner on 5/25/16.
//  Copyright © 2016 Harry Kalodner. All rights reserved.
//

#include "utils.hpp"

#include <random>

inline std::mt19937& getGen() {
    static std::random_device *rd = new std::random_device();
    static std::mt19937 gen((*rd)());
    return gen;
}

std::size_t selectRandomIndex(size_t size) {
    if (size == 1) {
        return 0;
    }
    std::uniform_int_distribution<size_t> vectorDis(0, size - 1);
    return vectorDis(getGen());
}

double selectRandomChance() {
    static std::uniform_real_distribution<double> dis(0, 1);
    return dis(getGen());
}

BlockTime selectMiningOffset(TimeRate mean) {
    std::geometric_distribution<unsigned long> dis(rawRate(mean));
    return BlockTime(dis(getGen()));
}

Value valWithNoise(Value minVal, Value maxVal) {
    static std::random_device *rd = new std::random_device();
    static std::mt19937 gen((*rd)());
    
    std::uniform_int_distribution<ValueType> dis(((rawValue(maxVal) - rawValue(minVal)) * 3) / 4 + rawValue(minVal), rawValue(maxVal));
    return Value(dis(gen));   //random between 75% maxVal and minVal
}
