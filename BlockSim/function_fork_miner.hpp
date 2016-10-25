//
//  function_fork_miner.hpp
//  BlockSim
//
//  Created by Harry Kalodner on 5/25/16.
//  Copyright © 2016 Harry Kalodner. All rights reserved.
//

#ifndef function_fork_miner_hpp
#define function_fork_miner_hpp

#include "typeDefs.hpp"

#include <memory>
#include <functional>

class Strategy;
class Blockchain;

using ForkFunc = std::function<Value(const Blockchain &, Value)>;

Strategy createFunctionForkStrategy(bool atomic, ForkFunc f, std::string type);

Value functionForkPercentage(const Blockchain &blockchain, Value maxVal, double funcCoeff);
Value functionForkLambert(const Blockchain &blockchain, Value maxVal, double lambertCoeff);

#endif /* function_fork_miner_hpp */
