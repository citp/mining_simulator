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

#include <vector>
#include <set>
#include <unordered_set>

class Miner;
class Blockchain;
class Block;

class MinerGroup {
private:
    bool modifiedSinceLastPublish;
    BlockTime nextBroadcastTime() const;
public:
    MinerGroup(std::vector<std::unique_ptr<Miner>> miners);
    
    void finalize(Blockchain &chain);
    void reset(const Blockchain &chain);
    
    const std::vector<std::unique_ptr<Miner>> miners;
    
    std::vector<Miner *> miningQueue;
    std::vector<std::unique_ptr<Block>> broadcastQueue;
    
    std::unordered_set<Miner *> activePublishers;
    std::unordered_set<Miner *> sleepingPublishers;
    
    friend std::ostream& operator<<(std::ostream& os, const MinerGroup& minerGroup);
    
    BlockTime nextEventTime(const Blockchain &chain);
    void nextMineRound(Blockchain &blockchain);
    void nextPublishRound(Blockchain &blockchain);
    void nextBroadcastRound(Blockchain &blockchain);
    
    void resetOrder();
};

#endif /* MinerGroup_hpp */
