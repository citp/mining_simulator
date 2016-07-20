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

class Miner;
class Blockchain;
class Block;
class Strategy;

Strategy createUndercutStrategy(bool noSelfMining, bool noiseInTransactions);

Block &undercutblockToMineOn(const Miner &me, const Blockchain &blockchain, bool noSelfMining);

#endif /* undercut_miner_hpp */
