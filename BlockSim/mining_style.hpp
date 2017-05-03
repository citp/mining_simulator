//
//  mining.hpp
//  BlockSim
//
//  Created by Harry Kalodner on 6/11/16.
//  Copyright © 2016 Harry Kalodner. All rights reserved.
//

#ifndef mining_hpp
#define mining_hpp

#include "typeDefs.hpp"

#include <functional>

class Blockchain;
class Miner;
class Block;

using ParentSelectorFunc = std::function<Block &(const Miner &, const Blockchain &)>;
using BlockValueFunc = std::function<Value(const Blockchain &, const Block &)>;

class MiningStyle {

private:
    const ParentSelectorFunc parentSelectorFunc;
    const BlockValueFunc blockValueFunc;
    
protected:
    MiningStyle(ParentSelectorFunc parentSelectorFunc, BlockValueFunc blockValueFunc);
    
    std::unique_ptr<Block> createBlock(Blockchain &blockchain, const Miner &me);
    
public:
    
    virtual ~MiningStyle();
    
    virtual std::pair<std::unique_ptr<Block>, Value> attemptToMine(Blockchain &blockchain, Miner *miner, BlockTime lastTimePaid) = 0;
    virtual BlockTime nextMiningTime(const Blockchain &chain, const Miner &miner) const = 0;
    virtual Value resetMiningCost(const Miner &miner, const Blockchain &chain, BlockTime lastTimePaid) = 0;
};

#endif /* mining_hpp */
