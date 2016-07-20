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
    std::vector< std::unique_ptr<MinedBlock> > unpublishedBlocks;
    
    BlockTime nextPublishingTime() const override;
    std::vector<std::unique_ptr<MinedBlock>> publishBlocks(const Blockchain &blockchain, const Miner &me) override;
    void addNewBlock(std::unique_ptr<MinedBlock> block) override;
    
    virtual bool publishTest(const Blockchain &blockchain, MinedBlock &block, const Miner &me) const = 0;
    
public:
    WithholdingPublisher() : PublishingStrategy() {}
    virtual ~WithholdingPublisher() = default;
};

#endif /* withholding_publisher_hpp */
