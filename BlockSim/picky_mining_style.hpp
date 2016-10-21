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

using ShouldMineFunc = std::function<bool(const Miner &, const Blockchain &, const MinedBlock &)>;

class PickyMiningStyle : public MiningStyle {
    
private:
    ShouldMineFunc shouldMineFunc;
    Value totalMiningCost;
    
    void initialize(const Blockchain &blockchain, const Miner &miner) override;
    BlockTime nextMiningTime() const override;
    Value moneySpentMining(const Miner &miner) const override;
    std::unique_ptr<MinedBlock> attemptToMineImp(const Blockchain &blockchain, Miner &me) override;
    
public:
    PickyMiningStyle(ParentSelectorFunc parentSelectorFunc_, BlockValueFunc blockValueFunc_, ShouldMineFunc shouldMineFunc_) : MiningStyle(parentSelectorFunc_, blockValueFunc_), shouldMineFunc(shouldMineFunc_)  {}
};

#endif /* picky_miner_hpp */
