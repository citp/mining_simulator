//
//  minerImp.hpp
//  BlockSim
//
//  Created by Harry Kalodner on 10/17/16.
//  Copyright © 2016 Harry Kalodner. All rights reserved.
//

#ifndef minerImp_hpp
#define minerImp_hpp

#include "typeDefs.hpp"
#include "mining_style.hpp"
#include "picky_mining_style.hpp"

#include <memory>
#include <functional>


class Block;
class MinedBlock;
class Blockchain;
class Miner;
class MiningStyle;
class PublishingStrategy;

class MinerImp {
public:
    std::unique_ptr<MiningStyle> miningStyle;
    std::unique_ptr<PublishingStrategy> publisher;
    
    MinerImp(ParentSelectorFunc parentSelectorFunc, BlockValueFunc blockValueFunc);
    MinerImp(ParentSelectorFunc parentSelectorFunc, BlockValueFunc blockValueFunc, ShouldMineFunc shouldMineFunc);
    MinerImp(ParentSelectorFunc parentSelectorFunc_, BlockValueFunc blockValueFunc_, ShouldMineFunc shouldMineFunc_, std::unique_ptr<PublishingStrategy> publisher_);
    MinerImp(ParentSelectorFunc parentSelectorFunc, BlockValueFunc blockValueFunc, std::unique_ptr<PublishingStrategy> publisher);
    MinerImp(std::unique_ptr<MiningStyle> miningStyle, std::unique_ptr<PublishingStrategy> publisher);
    
    void initialize(const Blockchain &, const Miner &);
};

#endif /* minerImp_hpp */
