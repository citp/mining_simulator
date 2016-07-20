//
//  withholding_publisher.cpp
//  BlockSim
//
//  Created by Harry Kalodner on 6/11/16.
//  Copyright © 2016 Harry Kalodner. All rights reserved.
//

#include "withholding_publisher.hpp"
#include "block.hpp"
#include "blockchain.hpp"

#include <assert.h>

BlockTime WithholdingPublisher::nextPublishingTime() const {
    if (unpublishedBlocks.size() > 0) {
        return getTimeReached() + BlockTime(1);
    } else {
        return BlockTime(std::numeric_limits<unsigned long>::max());
    }
}

void WithholdingPublisher::addNewBlock(std::unique_ptr<MinedBlock> block) {
    unpublishedBlocks.push_back(std::move(block));
}

std::vector<std::unique_ptr<MinedBlock>> WithholdingPublisher::publishBlocks(const Blockchain &blockchain, const Miner &me) {
    std::vector<std::unique_ptr<MinedBlock>> blocksToPublish;
    bool donePublishing = false;
    while (!donePublishing) {
        donePublishing = true;
        for (auto iter = begin(unpublishedBlocks); iter != end(unpublishedBlocks); iter++) {
            assert(iter->get() != nullptr);
            if (publishTest(blockchain, **iter, me)) {
                blocksToPublish.push_back(std::move(*iter));
                unpublishedBlocks.erase(iter);
                donePublishing = false;
                break;
            }
        }
    }
    std::sort(begin(blocksToPublish), end(blocksToPublish), [](const std::unique_ptr<MinedBlock> &a, const std::unique_ptr<MinedBlock> &b) { return a->height < b->height; });
    return blocksToPublish;
}
