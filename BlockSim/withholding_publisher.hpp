//
//  withholding_publisher.hpp
//  BlockSim
//
//  Created by Harry Kalodner on 6/11/16.
//  Copyright © 2016 Harry Kalodner. All rights reserved.
//

#ifndef withholding_publisher_hpp
#define withholding_publisher_hpp

#include "publishing_strategy.hpp"

class Miner;

class WithholdingPublisher : public PublishingStrategy {
    
private:
    std::vector<std::unique_ptr<Block>> publishBlocks(const Blockchain &blockchain, const Miner &me, std::vector<std::unique_ptr<Block>> &unpublishedBlocks) override;
    
    virtual bool publishTest(const Blockchain &blockchain, Block &block, const Miner &me) const = 0;
    
    bool withholdsBlocks() const override { return true; }
    
public:
    WithholdingPublisher() : PublishingStrategy() {}
    virtual ~WithholdingPublisher() = default;
};

#endif /* withholding_publisher_hpp */
