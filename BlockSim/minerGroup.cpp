//
//  minerGroup.cpp
//  BlockSim
//
//  Created by Harry Kalodner on 6/9/16.
//  Copyright © 2016 Harry Kalodner. All rights reserved.
//

#include "minerGroup.hpp"

#include "miner.hpp"
#include "blockchain.hpp"
#include "block.hpp"
#include "logging.h"

#include <cassert>
#include <iostream>
#include <limits>

constexpr auto maxTime = BlockTime(std::numeric_limits<BlockTime>::max());

bool miningSort(const Miner *miner1, const Miner *miner2);
bool miningSort(const Miner *miner1, const Miner *miner2)  {
    
    auto miner1Time = miner1->nextMiningTime();
    auto miner2Time = miner2->nextMiningTime();
    return miner1Time > miner2Time;
}

bool broadcastSort(const std::unique_ptr<Block> &block1, const std::unique_ptr<Block> &block2);
bool broadcastSort(const std::unique_ptr<Block> &block1, const std::unique_ptr<Block> &block2)  {
    return block1->getTimeBroadcast() > block2->getTimeBroadcast();
}


MinerGroup::MinerGroup(std::vector<std::unique_ptr<Miner>> miners_) : modifiedSinceLastPublish(false), miners(std::move(miners_)) {
    for (auto &miner : miners) {
        miningQueue.push_back(miner.get());
        sleepingPublishers.insert(miner.get());
    }
}

void MinerGroup::reset(const Blockchain &chain) {
    for (auto &miner : miners) {
        miner->reset(chain);
    }
    broadcastQueue.clear();
    
    std::make_heap(begin(miningQueue), end(miningQueue), miningSort);
}

void MinerGroup::finalize(Blockchain &chain) {
    for (auto &miner : miners) {
        miner->finalize(chain);
    }
}

BlockTime MinerGroup::nextBroadcastTime() const {
    if (!broadcastQueue.empty()) {
        return broadcastQueue.front()->getTimeBroadcast();
    } else {
        return maxTime;
    }
}

BlockTime MinerGroup::nextEventTime(const Blockchain &chain) {
    
    auto nextMining = miningQueue.front()->nextMiningTime();
    
    if (modifiedSinceLastPublish) {
        return chain.getTime() + BlockTime(1);
    } else {
        return std::min(nextBroadcastTime(), nextMining);
    }
}

void MinerGroup::nextMineRound(Blockchain &blockchain) {
    assert(miningQueue.front()->nextMiningTime() >= blockchain.getTime());
    
    while (miningQueue.front()->nextMiningTime() == blockchain.getTime()) {
        std::pop_heap(begin(miningQueue), end(miningQueue), miningSort);
        Miner *miner = miningQueue.back();
        auto wantedBroadcast = miner->wantsToBroadcast();
        auto block = miner->miningPhase(blockchain);
        if (block) {
            COMMENTARY("Miner " << miner->params.name << " publishes " << *block << "\n");
            broadcastQueue.push_back(std::move(block));
            std::push_heap(begin(broadcastQueue), end(broadcastQueue), broadcastSort);
        }
        if (miner->wantsToBroadcast() && !wantedBroadcast) {
            sleepingPublishers.erase(miner);
            activePublishers.insert(miner);
        }
        
        std::push_heap(begin(miningQueue), end(miningQueue), miningSort);
    }
    modifiedSinceLastPublish = true;
}

void MinerGroup::nextBroadcastRound(Blockchain &blockchain) {
    while (!broadcastQueue.empty() && broadcastQueue.front()->getTimeBroadcast() == blockchain.getTime()) {
        std::pop_heap(begin(broadcastQueue), end(broadcastQueue), broadcastSort);
        blockchain.publishBlock(std::move(broadcastQueue.back()));
        broadcastQueue.pop_back();
    }
}

void MinerGroup::nextPublishRound(Blockchain &blockchain) {
    if (modifiedSinceLastPublish) {
        bool wasModified = true;
        while (wasModified) {
            wasModified = false;
            for (auto it = begin(activePublishers); it != end(activePublishers); ) {
                auto blocksToBroadcast = (*it)->broadcastPhase(blockchain);
                for (auto &block : blocksToBroadcast) {
                    broadcastQueue.push_back(std::move(block));
                    std::push_heap(begin(broadcastQueue), end(broadcastQueue), broadcastSort);
                    wasModified = true;
                }
                if (!(*it)->wantsToBroadcast()) {
                    sleepingPublishers.insert(*it);
                    it = activePublishers.erase(it);
                } else {
                    ++it;
                }
            }
            nextBroadcastRound(blockchain);
        }
    }
    
    modifiedSinceLastPublish = false;
}

std::ostream& operator<<(std::ostream& os, const MinerGroup& minerGroup) {
    for (const auto &miner : minerGroup.miners) {
        os << *miner << std::endl;
    }
    return os;
}

void MinerGroup::resetOrder() {
    std::make_heap(begin(miningQueue), end(miningQueue), miningSort);
    activePublishers.clear();
    sleepingPublishers.clear();
    for (auto &miner : miners) {
        if (miner->wantsToBroadcast()) {
            activePublishers.insert(miner.get());
        } else {
            sleepingPublishers.insert(miner.get());
        }
    }
}
