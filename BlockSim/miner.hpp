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

using ParentSelectorFunc = std::function<Block &(const Miner &, const Blockchain &)>;
using BlockValueFunc = std::function<Value(const Blockchain &, const Block &)>;
using ShouldMineFunc = std::function<bool(const Miner &, const Blockchain &, const MinedBlock &)>;

class Miner final {

private:
    BlockCount blocksMinedTotal;
    std::deque<std::unique_ptr<MinedBlock>> waitingToPublishQueue;
    optional<std::reference_wrapper<MinedBlock>> _lastMinedBlock;

    bool findsBlock(const Blockchain &blockchain);
    
    std::unique_ptr<MiningStyle> miningStyle;
    std::unique_ptr<PublishingStrategy> publisher;
public:
    const MinerParameters params;
    const Strategy &strat;
    
    
    Miner(MinerParameters parameters, const Strategy &strat, ParentSelectorFunc parentSelectorFunc, BlockValueFunc blockValueFunc);
    Miner(MinerParameters parameters, const Strategy &strat, ParentSelectorFunc parentSelectorFunc, BlockValueFunc blockValueFunc, ShouldMineFunc shouldMineFunc);
    
    Miner(MinerParameters parameters_, const Strategy &strat, ParentSelectorFunc parentSelectorFunc_, BlockValueFunc blockValueFunc_, ShouldMineFunc shouldMineFunc_, std::unique_ptr<PublishingStrategy> publisher_);
    Miner(MinerParameters parameters, const Strategy &strat, ParentSelectorFunc parentSelectorFunc, BlockValueFunc blockValueFunc, std::unique_ptr<PublishingStrategy> publisher);
    
    Miner(MinerParameters parameters_, const Strategy &strat, std::unique_ptr<MiningStyle> miningStyle_, std::unique_ptr<PublishingStrategy> publisher_);
    void initialize(const Blockchain &blockchain);
    
    BlockCount getBlocksMinedTotal() const { return blocksMinedTotal; }
    
    BlockTime nextMiningTime() const;
    BlockTime nextPublishingTime() const;
    
    void publishPhase(Blockchain &blockchain);
    void miningPhase(const Blockchain &blockchain);
    
    optional<std::reference_wrapper<MinedBlock>> getLastMinedBlock() const { return _lastMinedBlock; }
    
    friend std::ostream& operator<<(std::ostream& os, const Miner& miner);
};

bool ownBlock(const Miner &miner, const Block &block);


#endif /* miner_hpp */
