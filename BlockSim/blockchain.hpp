//
//  blockchain.hpp
//  BlockSim
//
//  Created by Harry Kalodner on 5/25/16.
//  Copyright © 2016 Harry Kalodner. All rights reserved.
//

#ifndef blockchain_hpp
#define blockchain_hpp

#include "typeDefs.hpp"

#include <queue>

class Block;
class GenesisBlock;
class Miner;
struct BlockchainSettings;

class Blockchain {    
    Value valueNetworkTotal;
    BlockTime timeInSecs;
    BlockRate secondsPerBlock;
    ValueRate transactionFeeRate;
    
    
    BlockHeight _maxHeightPub;
    std::vector<std::vector<size_t>> _blocksIndex;
    std::vector<std::vector<Block *>> _smallestBlocks; // cache smallest blocks of a given height
    std::vector<std::unique_ptr<Block>> _blocks;
    
    std::vector<std::unique_ptr<Block>> _oldBlocks;
    
    Block *blockByMinerAtHeight(BlockHeight height, const Miner &miner) const;
    
public:
    Blockchain(BlockchainSettings blockchainSettings);
    
    std::unique_ptr<Block> createBlock(const Block *parent, const Miner *miner, Value value);
    void reset(BlockchainSettings blockchainSettings);

    void publishBlock(std::unique_ptr<Block> block);
    
    
    void printBlockchain() const;
    void printHeads() const;
    
    const Block &winningHead() const;
    
    BlockCount blocksOfHeight(BlockHeight height) const;
    
    const std::vector<Block *> oldestPublishedHeads(BlockHeight height) const;
    Block &oldestPublishedHead(BlockHeight height) const;
    Block &smallestHead(BlockHeight age) const;
    
    void advanceToTime(BlockTime time);
    
    inline BlockHeight getMaxHeightPub() const {
        return _maxHeightPub;
    }
    
    inline BlockTime getTime() const {
        return timeInSecs;
    }
    
    inline Value getTotalFees() const {
        return valueNetworkTotal;
    }
    
    BlockValue expectedBlockSize() const;
    TimeRate chanceToWin(HashRate hashRate) const;
    
    Value gap(BlockHeight i) const;
    Value rem(const Block &block) const;
    
    Block &most(BlockHeight age, const Miner &miner) const;
    Block &oldest(BlockHeight age, const Miner &miner) const;
};

#endif /* blockchain_hpp */
