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
#include "minerParameters.h"
#include "miner.hpp"

#include <functional>

class MinedBlock;
class Blockchain;
class Miner;

class MiningStyle {

private:
    BlockTime _miningTimeReached;
    ParentSelectorFunc parentSelectorFunc;
    BlockValueFunc blockValueFunc;
    
    virtual std::unique_ptr<MinedBlock> attemptToMineImp(const Blockchain &blockchain, Miner &miner) = 0;
    
protected:
    MiningStyle(ParentSelectorFunc parentSelectorFunc, BlockValueFunc blockValueFunc);
    
    std::unique_ptr<MinedBlock> createBlock(const Blockchain &blockchain, const Miner &me);
    
public:
    
    inline BlockTime getTimeReached() const { return _miningTimeReached; }
    
    virtual ~MiningStyle();
    
    virtual void initialize(const Blockchain &, const Miner &) {}
    std::unique_ptr<MinedBlock> attemptToMine(const Blockchain &blockchain, Miner *miner);
    virtual BlockTime nextMiningTime() const = 0;
    virtual Value moneySpentMining(const Miner &miner) const = 0;
};

#endif /* mining_hpp */
