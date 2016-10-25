//
//  default_miner.hpp
//  BlockSim
//
//  Created by Harry Kalodner on 5/25/16.
//  Copyright © 2016 Harry Kalodner. All rights reserved.
//

#ifndef default_miner_hpp
#define default_miner_hpp

#include "typeDefs.hpp"

#include <memory>

class Miner;
class Block;
class Blockchain;
class Strategy;

Strategy createDefaultStrategy(bool atomic, bool noiseInTransactions);

Value defaultValueInMinedChild(const Blockchain &blockchain, const Block &mineHere, bool noiseInTransactions);

Block &defaultBlockToMineOnAtomic(const Miner &me, const Blockchain &chain);
Block &defaultBlockToMineOnNonAtomic(const Miner &me, const Blockchain &chain);

#endif /* default_miner_hpp */
