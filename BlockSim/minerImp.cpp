//
//  minerImp.cpp
//  BlockSim
//
//  Created by Harry Kalodner on 10/17/16.
//  Copyright © 2016 Harry Kalodner. All rights reserved.
//

#include "minerImp.hpp"

#include "picky_mining_style.hpp"
#include "simple_mining_style.hpp"
#include "simple_publisher.hpp"
#include "block.hpp"

MinerImp::MinerImp(ParentSelectorFunc parentSelFunc_, BlockValueFunc blockValFunc_) :
MinerImp(parentSelFunc_, blockValFunc_, std::make_unique<SimplePublisher>()) {}

MinerImp::MinerImp(ParentSelectorFunc parentSelFunc_, BlockValueFunc blockValFunc_, ShouldMineFunc shouldMineFunc_) :
MinerImp(parentSelFunc_, blockValFunc_, shouldMineFunc_, std::make_unique<SimplePublisher>()) {}

MinerImp::MinerImp(ParentSelectorFunc parentSelFunc_, BlockValueFunc blockValFunc_, ShouldMineFunc shouldMineFunc_, std::unique_ptr<PublishingStrategy> publisher_) :
MinerImp(std::make_unique<PickyMiningStyle>(parentSelFunc_, blockValFunc_, shouldMineFunc_), std::move(publisher_)) {}

MinerImp::MinerImp(ParentSelectorFunc parentSelectorFunc, BlockValueFunc blockValueFunc, std::unique_ptr<PublishingStrategy> publisher_) :
MinerImp(std::make_unique<SimpleMiningStyle>(parentSelectorFunc, blockValueFunc), std::move(publisher_)) {}

MinerImp::MinerImp(std::unique_ptr<MiningStyle> miningStyle_, std::unique_ptr<PublishingStrategy> publisher_) : miningStyle(std::move(miningStyle_)), publisher(std::move(publisher_)) {}


void MinerImp::initialize(const Blockchain &blockchain, const Miner &miner) {
    miningStyle->initialize(blockchain, miner);
    publisher->initialize(blockchain, miner);
}
