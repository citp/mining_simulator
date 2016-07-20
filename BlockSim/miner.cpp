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

#include <cassert>
#include <string>
#include <utility>
#include <iostream>
#include <cmath>

Miner::Miner(MinerParameters parameters_, const Strategy &strat_, ParentSelectorFunc parentSelectorFunc_, BlockValueFunc blockValueFunc_) : Miner(parameters_, strat_, parentSelectorFunc_, blockValueFunc_, std::make_unique<SimplePublisher>()) {}

Miner::Miner(MinerParameters parameters_, const Strategy &strat_, ParentSelectorFunc parentSelectorFunc_, BlockValueFunc blockValueFunc_, ShouldMineFunc shouldMineFunc_) : Miner(parameters_, strat_, parentSelectorFunc_, blockValueFunc_, shouldMineFunc_, std::make_unique<SimplePublisher>()) {}

Miner::Miner(MinerParameters parameters_, const Strategy &strat_, ParentSelectorFunc parentSelectorFunc_, BlockValueFunc blockValueFunc_, ShouldMineFunc shouldMineFunc_, std::unique_ptr<PublishingStrategy> publisher_) : Miner(parameters_, strat_, std::make_unique<PickyMiningStyle>(parentSelectorFunc_, blockValueFunc_, shouldMineFunc_), std::move(publisher_)) {}

Miner::Miner(MinerParameters parameters_, const Strategy &strat_, ParentSelectorFunc parentSelectorFunc, BlockValueFunc blockValueFunc, std::unique_ptr<PublishingStrategy> publisher_) : Miner(parameters_, strat_, std::make_unique<SimpleMiningStyle>(parentSelectorFunc, blockValueFunc), std::move(publisher_)) {}

Miner::Miner(MinerParameters parameters_, const Strategy &strat_, std::unique_ptr<MiningStyle> miningStyle_, std::unique_ptr<PublishingStrategy> publisher_) :
    miningStyle(std::move(miningStyle_)),
    publisher(std::move(publisher_)),
    params(parameters_),
    strat(strat_) {}

void Miner::initialize(const Blockchain &blockchain) {
    miningStyle->initialize(blockchain, *this);
}

BlockTime Miner::nextMiningTime() const {
    return miningStyle->nextMiningTime();
}

void Miner::miningPhase(const Blockchain &blockchain) {

    assert(blockchain.getTime() <= miningStyle->nextMiningTime());
    
    COMMENTARY("\tMiner " << params.name << "'s turn. " << strat << ". ");
    
    auto block = miningStyle->attemptToMine(blockchain, this);
    
    COMMENTARYBLOCK (
        if (block) {
            COMMENTARY("\n### Miner " << params.name << " found a block: " << *block << " on top of " << block->parent << " with value " << block->value << " ###");
        }
    )
    
    if (block) {
        _lastMinedBlock = make_optional(std::ref(*block));
        publisher->addNewBlock(std::move(block));
        blocksMinedTotal++;
    }
    
    COMMENTARY("Turn over." << std::endl);
}

void Miner::publishPhase(Blockchain &blockchain) {
    
    if (publisher->nextPublishingTime() == blockchain.getTime()) {
        auto blocksToPublish = publisher->publish(blockchain, *this);
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
        return std::min(publisher->nextPublishingTime(), waitingToPublishQueue.front()->timeMined + params.networkDelay);
    } else {
        return publisher->nextPublishingTime();
    }
}

std::ostream& operator<<(std::ostream& os, const Miner& miner) {
    os << "[" << miner.strat << "] miner " << miner.params.name;
    return os;
}

bool ownBlock(const Miner &miner, const Block &block) {
    return block.minedBy(miner);
}
