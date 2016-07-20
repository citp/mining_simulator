//
//  mining.cpp
//  BlockSim
//
//  Created by Harry Kalodner on 6/11/16.
//  Copyright © 2016 Harry Kalodner. All rights reserved.
//

#include "mining_style.hpp"

#include "blockchain.hpp"
#include "miner.hpp"
#include "block.hpp"

#include <assert.h>

MiningStyle::MiningStyle(ParentSelectorFunc parentSelectorFunc_, BlockValueFunc blockValueFunc_) :
    _miningTimeReached(BlockTime(0)), parentSelectorFunc(parentSelectorFunc_), blockValueFunc(blockValueFunc_) {}

MiningStyle::~MiningStyle() = default;


std::unique_ptr<MinedBlock> MiningStyle::attemptToMine(const Blockchain &blockchain, Miner *miner) {
    assert(blockchain.getTime() <= nextMiningTime());
    assert(_miningTimeReached <= blockchain.getTime());
    
    _miningTimeReached = blockchain.getTime();
    
    auto block = attemptToMineImp(blockchain, *miner);
    
    _miningTimeReached = blockchain.getTime() + BlockTime(1);
    
    assert(nextMiningTime() > blockchain.getTime());
    
    return block;
}


std::unique_ptr<MinedBlock> MiningStyle::createBlock(const Blockchain &blockchain, const Miner &miner) {
    auto &parent = parentSelectorFunc(miner, blockchain);
    auto value = blockValueFunc(blockchain, parent);
    
    return std::make_unique<MinedBlock>(parent, miner, blockchain.getTime(), value);
}
