//
//  clever_miner.hpp
//  BlockSim
//
//  Created by Harry Kalodner on 5/25/16.
//  Copyright © 2016 Harry Kalodner. All rights reserved.
//

#ifndef clever_miner_hpp
#define clever_miner_hpp

#include "withholding_publisher.hpp"

class CleverPublishingStyle : public WithholdingPublisher {
private:
    
    bool publishTest(const Blockchain &blockchain, MinedBlock &block, const Miner &me) const override;
    
    using WithholdingPublisher::WithholdingPublisher;
};

Strategy createCleverForkStrategy(bool noSelfMining);

#endif /* clever_miner_hpp */
