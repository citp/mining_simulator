//
//  strategy.cpp
//  BlockSim
//
//  Created by Harry Kalodner on 5/31/16.
//  Copyright © 2016 Harry Kalodner. All rights reserved.
//

#include "strategy.hpp"
#include "publishing_strategy.hpp"
#include "simple_publisher.hpp"
#include "simple_mining_style.hpp"
#include "block.hpp"

#include <iostream>

Strategy::Strategy(std::string name_, ParentSelectorFunc parentSelFunc_, BlockValueFunc blockValFunc_) :
Strategy(name_, parentSelFunc_, blockValFunc_, std::make_unique<SimplePublisher>()) {}

Strategy::Strategy(std::string name_, ParentSelectorFunc parentSelFunc_, BlockValueFunc blockValFunc_, ShouldMineFunc shouldMineFunc_) :
Strategy(name_, parentSelFunc_, blockValFunc_, shouldMineFunc_, std::make_unique<SimplePublisher>()) {}

Strategy::Strategy(std::string name_, ParentSelectorFunc parentSelFunc_, BlockValueFunc blockValFunc_, ShouldMineFunc shouldMineFunc_, std::unique_ptr<PublishingStrategy> publisher_) :
Strategy(name_, std::make_unique<PickyMiningStyle>(parentSelFunc_, blockValFunc_, shouldMineFunc_), std::move(publisher_)) {}

Strategy::Strategy(std::string name_, ParentSelectorFunc parentSelectorFunc, BlockValueFunc blockValueFunc, std::unique_ptr<PublishingStrategy> publisher_) :
Strategy(name_, std::make_unique<SimpleMiningStyle>(parentSelectorFunc, blockValueFunc), std::move(publisher_)) {}

Strategy::Strategy(std::string name_, std::unique_ptr<MiningStyle> miningStyle_, std::unique_ptr<PublishingStrategy> publisher_) : name(name_), miningStyle(std::move(miningStyle_)), publisher(std::move(publisher_)) {}
