//
//  minerParameters.h
//  BlockSim
//
//  Created by Harry Kalodner on 6/7/16.
//  Copyright © 2016 Harry Kalodner. All rights reserved.
//

#ifndef minerParameters_h
#define minerParameters_h

#include "typeDefs.hpp"

#include <string>

struct MinerParameters {
    const unsigned int number;
    const std::string name;
    const HashRate hashRate;
    const BlockTime networkDelay;
    const ValueRate costPerSecond;
};

#endif /* minerParameters_h */
