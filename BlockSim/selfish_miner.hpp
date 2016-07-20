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

class Miner;
class Block;

class SelfishPublishingStyle : public PublishingStrategy {
private:
    BlockTime nextPublishingTime() const override;
    void addNewBlock(std::unique_ptr<MinedBlock> block) override;
    std::vector<std::unique_ptr<MinedBlock>> publishBlocks(const Blockchain &blockchain, const Miner &me) override;
protected:
    std::vector<std::unique_ptr<MinedBlock>> selfishChain;
    virtual BlockHeight heightToPublish(const Blockchain &blockchain, const Miner &me) const;
public:
    SelfishPublishingStyle();
    
};

Strategy createSelfishStrategy(bool noiseInTransactions);

Block &selfishBlockToMineOn(const Miner &me, const Blockchain &blockchain);

#endif /* selfish_miner_hpp */
