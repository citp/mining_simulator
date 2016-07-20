//
//  selfish_miner.cpp
//  BlockSim
//
//  Created by Harry Kalodner on 5/25/16.
//  Copyright © 2016 Harry Kalodner. All rights reserved.
//

#include "selfish_miner.hpp"
#include "block.hpp"
#include "blockchain.hpp"
#include "logging.h"
#include "miner.hpp"
#include "default_miner.hpp"
#include "mining_style.hpp"
#include "strategy.hpp"

#include <cmath>
#include <iostream>
#include <assert.h>

using std::placeholders::_1;
using std::placeholders::_2;

Strategy createSelfishStrategy(bool noiseInTransactions) {
    auto valueFunc = std::bind(defaultValueInMinedChild, _1, _2, noiseInTransactions);
    
    auto defaultCreator = [=](MinerParameters params, const Strategy &strat) { return std::make_unique<Miner>(params, strat, selfishBlockToMineOn, valueFunc, std::make_unique<SelfishPublishingStyle>()); };
    
    return {"selfish", defaultCreator};
}

Block &selfishBlockToMineOn(const Miner &me, const Blockchain &blockchain) {
    auto block = me.getLastMinedBlock();
    if (block && block->get().height >= blockchain.getMaxHeightPub()) {
        return *block;
    } else {
        return blockchain.oldestPublishedHead();
    }
}

SelfishPublishingStyle::SelfishPublishingStyle() : PublishingStrategy() {}

BlockTime SelfishPublishingStyle::nextPublishingTime() const {
    if (selfishChain.size() > 0) {
        return getTimeReached() + BlockTime(1);
    } else {
        return BlockTime(std::numeric_limits<unsigned long>::max());
    }
}

void SelfishPublishingStyle::addNewBlock(std::unique_ptr<MinedBlock> block) {
    selfishChain.push_back(std::move(block));
}

std::vector<std::unique_ptr<MinedBlock>> SelfishPublishingStyle::publishBlocks(const Blockchain &blockchain, const Miner &me) {
    assert(!selfishChain.empty());
    BlockHeight height = heightToPublish(blockchain, me);
    
    assert(height > BlockHeight(0));
    
    auto splitPoint = std::find_if(begin(selfishChain), end(selfishChain), [&](auto &p) { return p->height == height;});
    
    std::vector<std::unique_ptr<MinedBlock>> split_lo(std::make_move_iterator(begin(selfishChain)), std::make_move_iterator(splitPoint));
    std::vector<std::unique_ptr<MinedBlock>> split_hi(std::make_move_iterator(splitPoint), std::make_move_iterator(end(selfishChain)));
    selfishChain = std::move(split_hi);
    return split_lo;
}

BlockHeight SelfishPublishingStyle::heightToPublish(const Blockchain &blockchain, const Miner &me) const {
    assert(!selfishChain.empty());
    BlockHeight heightToPublish(0);
    if(selfishChain.back()->height == blockchain.getMaxHeightPub()) {
        //this represents when a selfish miner had a lead, but the network (honest miners, whathave you) found a block, bringing the public height of the block chain up to the height of the selfish miner's private chain. Want to publish immediately for a (gamma) chance to win (or the miner actually tied with another miner-- but they'd still want to publish immediately)
        COMMENTARY("Miner " << me.params.name << " publishes selfish chain for a race condition." << std::endl);
        heightToPublish = selfishChain.back()->height;
    } else if(selfishChain.back()->height == blockchain.getMaxHeightPub() + BlockHeight(1) && ownBlock(me, selfishChain.back()->parent)) {
        if(selfishChain.size() == 1) {
            //in this situation, you've gotta differentiate between when you found your first hidden block and if you are posting a block to win a race condition. You should publish if you're coming out of a race condition...
            //the way to know is if there is another public head at the same height as the head->parent().
            const auto &possibleHeads = blockchain.getHeadsOfHeight(blockchain.getMaxHeightPub());
            if (possibleHeads.size() > 1) {
                COMMENTARY("Miner " << me.params.name << " publishes selfish chain to protect past block.\n");
                heightToPublish = selfishChain.back()->height;
            }
        } else {
            //this is the case where the selfish miner mined at least 2 blocks, and is only 1 ahead of the public chain-- they should publish because they want to protect their past blocks
            COMMENTARY("Miner " << me.params.name << " publishes selfish chain to protect blocks.\n");
            heightToPublish = selfishChain.back()->height;
        }
    } else {
        // If we have more blocks in reserve just keep 2 blocks hidden
        heightToPublish = blockchain.getMaxHeightPub();
        COMMENTARY("Miner " << me.params.name << " publishes up to " << heightToPublish << " because other miners found block(s). " << selfishChain.back()->height - heightToPublish << " blocks remaining in chain." << std::endl);
    }
    return heightToPublish;
}
