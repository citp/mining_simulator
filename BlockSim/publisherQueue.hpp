//
//  publisherQueue.hpp
//  BlockSim
//
//  Created by Harry Kalodner on 6/19/16.
//  Copyright © 2016 Harry Kalodner. All rights reserved.
//

#ifndef publisherQueue_hpp
#define publisherQueue_hpp

#include "typeDefs.hpp"

#include <set>
#include <unordered_set>
#include <vector>
#include <memory>

class Miner;

struct publisher_compare {
    bool operator() (const Miner *miner1, const Miner *miner2) const;
};

class PublisherQueue {
    std::set<Miner *, publisher_compare> active;
    std::unordered_set<Miner *> waiting;
    
public:
    PublisherQueue(const std::vector<std::unique_ptr<Miner>> &miners);
    
    BlockTime nextPublishingTime() const;
    
    Miner *popNextPublisher();
    void pushPublisher(Miner *miner);
    void removePublisher(Miner *miner);
};

#endif /* publisherQueue_hpp */
