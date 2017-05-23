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

#include <queue>
#include <memory>
#include <experimental/optional>

using std::experimental::optional;
using std::experimental::make_optional;
using std::experimental::nullopt;

class Block;
class Blockchain;
class Strategy;

class Miner {

private:
    BlockCount blocksMinedTotal;
    std::vector<std::unique_ptr<Block>> unbroadcastBlocks;
    std::reference_wrapper<const Strategy> strategy;
    
    Value totalMiningCost;
    BlockTime _lastCostUpdate;
    BlockTime _nextMiningTime;
    BlockTime _nextPublishTime;
    bool waitingForBroadcast;
    
    bool findsBlock(const Blockchain &blockchain);
    void updateNextPublishTime(BlockTime newTime, const Blockchain &chain);
    
protected:
    virtual void print(std::ostream& where) const;
public:
    const MinerParameters params;
    
    Miner(MinerParameters parameters, const Strategy &strategy);
    virtual ~Miner();
    
    void changeStrategy(const Strategy &strategy, const Blockchain &blockchain);
    
    Block *newestUnpublishedBlock() const;
    
    void finalize(Blockchain &blockchain);
    void reset(const Blockchain &blockchain);
    bool wantsToBroadcast() const { return waitingForBroadcast; }
    
    inline BlockCount getBlocksMinedTotal() const { return blocksMinedTotal; }
    BlockTime nextMiningTime() const {  return _nextMiningTime; }
    
    bool publishesNextRound() const;
    
    std::unique_ptr<Block> miningPhase(Blockchain &blockchain);
    std::vector<std::unique_ptr<Block>> broadcastPhase(const Blockchain &chain);
    
    friend std::ostream& operator<<(std::ostream& os, const Miner& miner);
};

bool ownBlock(const Miner *miner, const Block *block);


#endif /* miner_hpp */
