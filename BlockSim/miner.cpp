//
//  miner.cpp
//  BlockSim
//
//  Created by Harry Kalodner on 5/25/16.
//  Copyright © 2016 Harry Kalodner. All rights reserved.
//

#include "miner.hpp"
#include "block.hpp"
#include "blockchain.hpp"
#include "logging.h"
#include "minerParameters.h"
#include "mining_style.hpp"
#include "simple_mining_style.hpp"
#include "picky_mining_style.hpp"
#include "publishing_strategy.hpp"
#include "simple_publisher.hpp"
#include "strategy.hpp"
#include "utils.hpp"

#include "minerImp.hpp"

#include <assert.h>
#include <string>
#include <utility>
#include <iostream>
#include <cmath>
#include <map>
#include <sstream>

Miner::Miner(MinerParameters params_, std::unique_ptr<MinerImp> imp_) : implementation(std::move(imp_)), params(params_) { }

Miner::~Miner() = default;

void Miner::changeImplementation(std::unique_ptr<MinerImp> implementation_) {
    implementation = std::move(implementation_);
}

void Miner::initialize(const Blockchain &blockchain) {
    implementation->initialize(blockchain, *this);
    blocksMinedTotal = BlockCount(0);
    waitingToPublishQueue.clear();
}

BlockTime Miner::nextMiningTime() const {
    return implementation->miningStyle->nextMiningTime();
}

void Miner::miningPhase(const Blockchain &blockchain) {

    assert(blockchain.getTime() <= implementation->miningStyle->nextMiningTime());
    
    COMMENTARY("\tMiner " << params.name << "'s turn. " << strat << ". ");
    
    auto block = implementation->miningStyle->attemptToMine(blockchain, this);
    
    COMMENTARYBLOCK (
        if (block) {
            COMMENTARY("\n### Miner " << params.name << " found a block: " << *block << " on top of " << block->parent << " with value " << block->value << " ###");
        }
    )
    
    if (block) {
        implementation->publisher->addNewBlock(std::move(block));
        blocksMinedTotal++;
    }
    
    COMMENTARY("Turn over." << std::endl);
}

void Miner::publishPhase(Blockchain &blockchain) {
    
    if (implementation->publisher->nextPublishingTime() == blockchain.getTime()) {
        auto blocksToPublish = implementation->publisher->publish(blockchain, *this);
        waitingToPublishQueue.insert(end(waitingToPublishQueue), std::make_move_iterator(begin(blocksToPublish)), std::make_move_iterator(end(blocksToPublish)));
    }
    
    assert(!waitingToPublishQueue.empty());
    assert(waitingToPublishQueue.front());
    
    while (!waitingToPublishQueue.empty() && params.networkDelay <= blockchain.getTime() - waitingToPublishQueue.front()->timeMined) {
        COMMENTARY("Miner " << params.name << " publishes " << *waitingToPublishQueue.front() << "." << std::endl);
        blockchain.publishBlock(std::move(waitingToPublishQueue.front()));
        waitingToPublishQueue.pop_front();
    }
}

BlockTime Miner::nextPublishingTime() const {
    if (!waitingToPublishQueue.empty()) {
        return std::min(implementation->publisher->nextPublishingTime(), waitingToPublishQueue.front()->timeMined + params.networkDelay);
    } else {
        return implementation->publisher->nextPublishingTime();
    }
}

std::ostream& operator<<(std::ostream& os, const Miner& miner) {
    miner.print(os);
    return os;
}

void Miner::print(std::ostream& os) const {
    os << "miner " << params.name;
}

bool ownBlock(const Miner &miner, const Block &block) {
    return block.minedBy(miner);
}

