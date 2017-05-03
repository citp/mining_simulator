//
//  undercut_miner.hpp
//  BlockSim
//
//  Created by Harry Kalodner on 5/25/16.
//  Copyright © 2016 Harry Kalodner. All rights reserved.
//

#ifndef undercut_miner_hpp
#define undercut_miner_hpp

#include <memory>

class Strategy;

std::unique_ptr<Strategy> createLazyForkStrategy(bool atomic);

#endif /* undercut_miner_hpp */
