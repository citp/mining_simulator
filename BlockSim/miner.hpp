//
//  miner.hpp
//  BlockSim
//
//  Created by Harry Kalodner on 5/25/16.
//  Copyright © 2016 Harry Kalodner. All rights reserved.
//

#ifndef miner_hpp
#define miner_hpp

#include "minerParameters.h"
#include "strategy.hpp"

#include <fstream>
#include <queue>
#include <functional>
#include <experimental/optional>

using std::experimental::optional;
using std::experimental::make_optional;
using std::experimental::nullopt;

class Block;
class MinedBlock;
class Blockchain;
class MiningStyle;
class PublishingStrategy;
class Miner;
class MinerImp;

class Miner {

private:
    BlockCount blocksMinedTotal;
    std::deque<std::unique_ptr<MinedBlock>> waitingToPublishQueue;
    std::unique_ptr<MinerImp> implementation;
    
    bool findsBlock(const Blockchain &blockchain);
protected:
    void changeImplementation(std::unique_ptr<MinerImp> implementation);
    virtual void print(std::ostream& where) const;
public:
    const MinerParameters params;
    
    Miner(MinerParameters parameters, std::unique_ptr<MinerImp> implementation);
    virtual ~Miner();
    
    void initialize(const Blockchain &blockchain);
    
    BlockCount getBlocksMinedTotal() const { return blocksMinedTotal; }
    
    BlockTime nextMiningTime() const;
    BlockTime nextPublishingTime() const;
    
    void publishPhase(Blockchain &blockchain);
    void miningPhase(const Blockchain &blockchain);
    
     friend std::ostream& operator<<(std::ostream& os, const Miner& miner);
};

bool ownBlock(const Miner &miner, const Block &block);


#endif /* miner_hpp */
