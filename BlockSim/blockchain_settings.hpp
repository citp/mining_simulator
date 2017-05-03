//
//  blockchain_settings.hpp
//  BlockSim
//
//  Created by Harry Kalodner on 10/26/16.
//  Copyright © 2016 Harry Kalodner. All rights reserved.
//

#ifndef blockchain_settings_hpp
#define blockchain_settings_hpp

#include "typeDefs.hpp"

struct BlockchainSettings {
    BlockRate secondsPerBlock;
    ValueRate transactionFeeRate;
    BlockValue blockReward;
    BlockCount numberOfBlocks;
};

#endif /* blockchain_settings_hpp */
