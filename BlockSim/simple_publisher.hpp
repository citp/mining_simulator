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
    
    BlockTime _nextPublish;
    
    std::unique_ptr<MinedBlock> unpublishedBlock;
    
    inline BlockTime nextPublishingTime() const override {
        return _nextPublish;
    }
    
    std::vector<std::unique_ptr<MinedBlock>> publishBlocks(const Blockchain &blockchain, const Miner &me) override;
    
    void addNewBlock(std::unique_ptr<MinedBlock> block) override;
public:
    SimplePublisher();
};

#endif /* simple_publisher_hpp */
