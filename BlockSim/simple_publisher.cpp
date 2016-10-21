//
//  simple_publisher.cpp
//  BlockSim
//
//  Created by Harry Kalodner on 6/11/16.
//  Copyright © 2016 Harry Kalodner. All rights reserved.
//

#include "simple_publisher.hpp"
#include "block.hpp"
#include "blockchain.hpp"

#include <assert.h>

SimplePublisher::SimplePublisher() : PublishingStrategy(),
    _nextPublish(std::numeric_limits<BlockTime>::max()) {}

std::vector<std::unique_ptr<MinedBlock>> SimplePublisher::publishBlocks(const Blockchain &, const Miner &) {
    assert(unpublishedBlock);
    _nextPublish = std::numeric_limits<BlockTime>::max();
    std::vector<std::unique_ptr<MinedBlock>> blocks;
    blocks.push_back(std::move(unpublishedBlock));
    return blocks;
}

void SimplePublisher::addNewBlock(std::unique_ptr<MinedBlock> block) {
    assert(!unpublishedBlock);
    unpublishedBlock = std::move(block);
    _nextPublish = unpublishedBlock->timeMined;
}

void SimplePublisher::initialize(const Blockchain &blockchain, const Miner &miner) {
    PublishingStrategy::initialize(blockchain, miner);
    _nextPublish = BlockTime(0);
    unpublishedBlock = nullptr;
}
