//
//  publisherQueue.cpp
//  BlockSim
//
//  Created by Harry Kalodner on 6/19/16.
//  Copyright © 2016 Harry Kalodner. All rights reserved.
//

#include "publisherQueue.hpp"

#include "miner.hpp"

constexpr auto maxTime = BlockTime(std::numeric_limits<BlockTime>::max());

bool publisher_compare::operator() (const Miner *miner1, const Miner *miner2) const {
    auto miner1Time = miner1->nextPublishingTime();
    auto miner2Time = miner2->nextPublishingTime();
    if (miner1Time < miner2Time) {
        return true;
    } else if (miner1Time > miner2Time) {
        return false;
    } else {
        return miner1->params.number < miner2->params.number;
    }
}

PublisherQueue::PublisherQueue(const std::vector<std::unique_ptr<Miner>> &miners) {
    std::vector<Miner *> minerPointers;
    minerPointers.reserve(miners.size());
    std::transform(begin(miners), end(miners), std::back_inserter(minerPointers), [](const auto &miner) { return miner.get(); });
    
    waiting.insert(begin(minerPointers), end(minerPointers));
}

BlockTime PublisherQueue::nextPublishingTime() const {
    if (active.empty()) {
        return maxTime;
    } else {
        return (*begin(active))->nextPublishingTime();
    }
}

Miner *PublisherQueue::popNextPublisher() {
    Miner *miner = *begin(active);
    active.erase(begin(active));
    return miner;
}

void PublisherQueue::pushPublisher(Miner *miner) {
    if (miner->nextPublishingTime() == maxTime) {
        waiting.insert(miner);
    } else {
        active.insert(miner);
    }
}

void PublisherQueue::removePublisher(Miner *miner) {
    auto it = waiting.find(miner);
    if (it != end(waiting)) {
        waiting.erase(it);
    } else {
        active.erase(miner);
    }
}
