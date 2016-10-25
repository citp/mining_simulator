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
class MinedBlock;
class Miner;

struct BlockchainSettings {
    BlockRate secondsPerBlock;
    ValueRate transactionFeeRate;
    BlockValue blockReward;
};

class Blockchain {
    const std::unique_ptr<GenesisBlock> genesis;
    
    std::vector<Block *> oldHeads;
    std::deque<Block *> heads;
    
    Value valueNetworkTotal;
    BlockTime timeInSecs;
    BlockRate secondsPerBlock;
    ValueRate transactionFeeRate;
    
    BlockHeight _maxHeightPub;
    std::vector<std::vector<Block *>> _smallestBlocks; // cache smallest blocks of a given height
    std::deque<std::vector<Block *>> _recentBlocksOfHeight;
    
private:
    void newBlockAdded(Block *block);
    
public:
    Blockchain(BlockchainSettings blockchainSettings);
    const std::deque<Block *> &getHeads() const;
    const std::vector<Block *> &getOldHeads() const;
    const std::vector<Block *> getHeadsOfHeight(BlockHeight height) const;
    
    void publishBlock(std::unique_ptr<MinedBlock> block);
    
    
    void printBlockchain() const;
    void printHeads() const;
    void printOldHeads() const;
    
    const Block &winningHead() const;
    
    const std::vector<Block *> oldestPublishedHeads(BlockHeight height) const;
    Block &oldestPublishedHead(BlockHeight height) const;
    Block &smallestHead(BlockHeight age) const;
    
    void tick(BlockTime timePassed);
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
