//
//  function_fork_selfish_miner.hpp
//  BlockSim
//
//  Created by Harry Kalodner on 11/4/16.
//  Copyright © 2016 Harry Kalodner. All rights reserved.
//

#ifndef function_fork_selfish_miner_hpp
#define function_fork_selfish_miner_hpp

#include "function_fork_miner.hpp"


std::unique_ptr<Strategy> createFunctionForkSelfishStrategy(Value cutoff, ForkFunc func);

#endif /* function_fork_selfish_miner_hpp */
