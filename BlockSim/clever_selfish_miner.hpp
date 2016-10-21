//
//  clevel_selfish_miner.hpp
//  BlockSim
//
//  Created by Harry Kalodner on 6/6/16.
//  Copyright © 2016 Harry Kalodner. All rights reserved.
//

#ifndef clevel_selfish_miner_hpp
#define clevel_selfish_miner_hpp

#include "selfish_miner.hpp"

class CleverSelfishPublishingStyle : public SelfishPublishingStyle {
private:
    const Value cutoff;
    BlockHeight heightToPublish(const Blockchain &blockchain, const Miner &me) const override;
public:
    CleverSelfishPublishingStyle(Value cutoff);
};

Strategy createCleverSelfishStrategy(bool noiseInTransactions, Value cutoff);

#endif /* clevel_selfish_miner_hpp */
