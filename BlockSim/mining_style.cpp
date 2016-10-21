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
parentSelectorFunc(parentSelectorFunc_), blockValueFunc(blockValueFunc_), _miningTimeReached(BlockTime(0)) {}

MiningStyle::~MiningStyle() = default;

void MiningStyle::initialize(const Blockchain &, const Miner &) {
    _miningTimeReached = BlockTime(0);
}


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
    
    auto newBlock = std::make_unique<MinedBlock>(parent, miner, blockchain.getTime(), value);
    
    assert(newBlock->height <= blockchain.getMaxHeightPub() + BlockHeight(20));
    
    return newBlock;
}
