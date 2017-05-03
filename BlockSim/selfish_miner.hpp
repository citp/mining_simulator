//
//  selfish_miner.hpp
//  BlockSim
//
//  Created by Harry Kalodner on 5/25/16.
//  Copyright © 2016 Harry Kalodner. All rights reserved.
//

#ifndef selfish_miner_hpp
#define selfish_miner_hpp

#include "publishing_strategy.hpp"

#include <memory>

class Block;
class Strategy;

class SelfishPublishingStyle : public PublishingStrategy {
private:
    std::vector<std::unique_ptr<Block>> publishBlocks(const Blockchain &blockchain, const Miner &me, std::vector<std::unique_ptr<Block>> &unpublishedBlocks) override;
    bool withholdsBlocks() const override { return true; }
    
    BlockHeight getPrivateHeadHeight(std::vector<std::unique_ptr<Block>> &unpublishedBlocks) const;
    
protected:
    virtual BlockHeight heightToPublish(const Blockchain &blockchain, const Miner &me, std::vector<std::unique_ptr<Block>> &unpublishedBlocks) const;
};

std::unique_ptr<Strategy> createSelfishStrategy(bool noiseInTransactions);
Block &selfishBlockToMineOn(const Miner &me, const Blockchain &blockchain);

#endif /* selfish_miner_hpp */
