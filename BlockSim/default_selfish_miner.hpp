//
//  default_selfish_miner.hpp
//  BlockSim
//
//  Created by Harry Kalodner on 6/6/16.
//  Copyright © 2016 Harry Kalodner. All rights reserved.
//

#ifndef default_selfish_miner_hpp
#define default_selfish_miner_hpp

#include <memory>

class Strategy;

Strategy createDefaultSelfishStrategy(bool noiseInTransactions, double gamma);

#endif /* default_selfish_miner_hpp */
