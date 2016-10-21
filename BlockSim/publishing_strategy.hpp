//
//  publisher.hpp
//  BlockSim
//
//  Created by Harry Kalodner on 6/11/16.
//  Copyright © 2016 Harry Kalodner. All rights reserved.
//

#ifndef publisher_hpp
#define publisher_hpp

#include "typeDefs.hpp"
#include "minerParameters.h"

#include <vector>

class MinedBlock;
class Blockchain;
class Miner;

class PublishingStrategy {
    
private:
    BlockTime publishingTimeReached;
    
protected:
    BlockTime getTimeReached() const;
    PublishingStrategy() : publishingTimeReached(BlockTime(0)) {}
    
    // Vector of blocks should be already topologically sorted
    virtual std::vector<std::unique_ptr<MinedBlock>> publishBlocks(const Blockchain &blockchain, const Miner &me) = 0;
public:
    std::vector<std::unique_ptr<MinedBlock>> publish(const Blockchain &blockchain, const Miner &me);
    
    virtual ~PublishingStrategy();
    virtual void initialize(const Blockchain &, const Miner &);
    virtual BlockTime nextPublishingTime() const = 0;
    virtual void addNewBlock(std::unique_ptr<MinedBlock> block) = 0;
};

#endif /* publisher_hpp */
