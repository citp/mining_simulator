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

#include <vector>
#include <memory>

class Block;
class Blockchain;
class Miner;

class PublishingStrategy {
public:
    virtual std::vector<std::unique_ptr<Block>> publishBlocks(const Blockchain &blockchain, const Miner &me, std::vector<std::unique_ptr<Block>> &unpublishedBlocks) = 0;
    virtual ~PublishingStrategy();
    
    virtual bool withholdsBlocks() const = 0;
};

#endif /* publisher_hpp */
