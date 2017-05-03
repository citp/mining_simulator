//
//  picky_miner.hpp
//  BlockSim
//
//  Created by Harry Kalodner on 6/12/16.
//  Copyright © 2016 Harry Kalodner. All rights reserved.
//

#ifndef picky_miner_hpp
#define picky_miner_hpp

#include "mining_style.hpp"

using ShouldMineFunc = std::function<bool(const Miner &, const Blockchain &, const Block &)>;

class PickyMiningStyle : public MiningStyle {
    
private:
    const ShouldMineFunc shouldMineFunc;
    
    BlockTime nextMiningTime(const Blockchain &chain, const Miner &miner) const override;
    std::pair<std::unique_ptr<Block>, Value> attemptToMine(Blockchain &blockchain, Miner *miner, BlockTime lastTimePaid) override;
    Value resetMiningCost(const Miner &miner, const Blockchain &chain, BlockTime lastTimePaid) override;
public:
    PickyMiningStyle(ParentSelectorFunc parentSelectorFunc, BlockValueFunc blockValueFunc, ShouldMineFunc shouldMineFunc);
};

#endif /* picky_miner_hpp */
