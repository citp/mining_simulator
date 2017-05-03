//
//  simple_miner.hpp
//  BlockSim
//
//  Created by Harry Kalodner on 6/11/16.
//  Copyright © 2016 Harry Kalodner. All rights reserved.
//

#ifndef simple_miner_hpp
#define simple_miner_hpp

#include "mining_style.hpp"

class SimpleMiningStyle : public MiningStyle {
    
private:
    std::pair<std::unique_ptr<Block>, Value> attemptToMine(Blockchain &blockchain, Miner *miner, BlockTime lastTimePaid) override;
    BlockTime nextMiningTime(const Blockchain &chain, const Miner &miner) const override;
    Value resetMiningCost(const Miner &miner, const Blockchain &chain, BlockTime lastTimePaid) override;
    
public:
    SimpleMiningStyle(ParentSelectorFunc parentSelectorFunc, BlockValueFunc blockValueFunc);
};

#endif /* simple_miner_hpp */
