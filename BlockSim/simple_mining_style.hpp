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
    bool _hasInitializedTime;
    BlockTime _nextBlockTime;
    
    BlockTime generateNewMiningTime(const Blockchain &blockchain, const Miner &miner);
    
    void initialize(const Blockchain &blockchain, const Miner &miner) override;
    std::unique_ptr<MinedBlock> attemptToMineImp(const Blockchain &blockchain, Miner &me) override;
    BlockTime nextMiningTime() const override;
    Value moneySpentMining(const Miner &miner) const override;
    
    
public:
    SimpleMiningStyle(ParentSelectorFunc parentSelectorFunc_, BlockValueFunc blockValueFunc_) : MiningStyle(parentSelectorFunc_, blockValueFunc_), _hasInitializedTime(false) {}
    virtual ~SimpleMiningStyle() = default;
};

#endif /* simple_miner_hpp */
