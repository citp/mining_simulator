//
//  simple_publisher.hpp
//  BlockSim
//
//  Created by Harry Kalodner on 6/11/16.
//  Copyright © 2016 Harry Kalodner. All rights reserved.
//

#ifndef simple_publisher_hpp
#define simple_publisher_hpp

#include "publishing_strategy.hpp"

class SimplePublisher : public PublishingStrategy {
    
private:
    std::vector<std::unique_ptr<Block>> publishBlocks(const Blockchain &, const Miner &, std::vector<std::unique_ptr<Block>> &) override;
    
    bool withholdsBlocks() const override { return false; }
public:
    SimplePublisher();
};

#endif /* simple_publisher_hpp */
