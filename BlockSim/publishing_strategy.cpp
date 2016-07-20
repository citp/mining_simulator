//
//  publisher.cpp
//  BlockSim
//
//  Created by Harry Kalodner on 6/11/16.
//  Copyright © 2016 Harry Kalodner. All rights reserved.
//

#include "publishing_strategy.hpp"

#include "blockchain.hpp"
#include "miner.hpp"
#include "block.hpp"

#include <assert.h>

PublishingStrategy::~PublishingStrategy() = default;

std::vector<std::unique_ptr<MinedBlock>> PublishingStrategy::publish(const Blockchain &blockchain, const Miner &me) {
    assert(blockchain.getTime() <= nextPublishingTime());
    assert(publishingTimeReached <= blockchain.getTime());
    
    std::vector<std::unique_ptr<MinedBlock>> blocksToPublish = publishBlocks(blockchain, me);
    
    for (const auto &block : blocksToPublish) {
        assert(block);
    }
    
    publishingTimeReached = blockchain.getTime() + BlockTime(1);
    
    return blocksToPublish;
}

BlockTime PublishingStrategy::getTimeReached() const {
    return publishingTimeReached;
}
