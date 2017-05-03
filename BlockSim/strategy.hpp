//
//  strategy.hpp
//  BlockSim
//
//  Created by Harry Kalodner on 5/31/16.
//  Copyright © 2016 Harry Kalodner. All rights reserved.
//

#ifndef strategy_hpp
#define strategy_hpp

#include "mining_style.hpp"
#include "picky_mining_style.hpp"
#include "publishing_strategy.hpp"

#include <functional>
#include <memory>
#include <string>

class MiningStyle;

class Strategy {
public:
    
    Strategy(std::string name, ParentSelectorFunc parentSelectorFunc, BlockValueFunc blockValueFunc);
    Strategy(std::string name, ParentSelectorFunc parentSelectorFunc, BlockValueFunc blockValueFunc, ShouldMineFunc shouldMineFunc);
    Strategy(std::string name, ParentSelectorFunc parentSelectorFunc_, BlockValueFunc blockValueFunc_, ShouldMineFunc shouldMineFunc_, std::unique_ptr<PublishingStrategy> publisher_);
    Strategy(std::string name, ParentSelectorFunc parentSelectorFunc, BlockValueFunc blockValueFunc, std::unique_ptr<PublishingStrategy> publisher);
    Strategy(std::string name, std::unique_ptr<MiningStyle> miningStyle, std::unique_ptr<PublishingStrategy> publisher);
    
    const std::string name;
    const std::unique_ptr<MiningStyle> miningStyle;
    const std::unique_ptr<PublishingStrategy> publisher;
    
};

#endif /* strategy_hpp */
