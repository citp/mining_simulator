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

SimplePublisher::SimplePublisher() : PublishingStrategy() {}

std::vector<std::unique_ptr<Block>> SimplePublisher::publishBlocks(const Blockchain &, const Miner &, std::vector<std::unique_ptr<Block>> &unpublishedBlocks) {
    std::vector<std::unique_ptr<Block>> blocks = std::move(unpublishedBlocks);
    unpublishedBlocks.clear();
    return blocks;
}
