//
//  minerGroup.hpp
//  BlockSim
//
//  Created by Harry Kalodner on 6/9/16.
//  Copyright © 2016 Harry Kalodner. All rights reserved.
//

#ifndef minerGroup_hpp
#define minerGroup_hpp

#include "typeDefs.hpp"
#include "publisherQueue.hpp"

#include <vector>

class Miner;
class Blockchain;
class MultiplicativeWeights;


class MinerGroup {
private:
    friend class MultiplicativeWeights;
public:
    MinerGroup(std::vector<std::unique_ptr<Miner>> miners);
    
    void initialize(const Blockchain &blockchain);
    
    std::vector<std::unique_ptr<Miner>> miners;
    PublisherQueue publisherQueue;
    
    friend std::ostream& operator<<(std::ostream& os, const MinerGroup& minerGroup);
    
    BlockTime nextEventTime();
    void nextMineRound(const Blockchain &blockchain);
    void nextPublishRound(Blockchain &blockchain);
    
};

#endif /* MinerGroup_hpp */
