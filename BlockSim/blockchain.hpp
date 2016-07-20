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
class MinedBlock;

class Blockchain {
    const std::unique_ptr<Block> genesis;
    
    std::vector<Block *> oldHeads;
    std::deque<Block *> heads;
    
    Value valueNetworkTotal;
    BlockTime timeInSecs;
    BlockRate secondsPerBlock;
    ValueRate transactionFeeRate;
    
    BlockHeight _maxHeightPub;
    std::vector<std::vector<Block *>> _smallestBlocks; // cache smallest blocks of a given height
    
private:
    void newBlockAdded(Block *block);
    
public:
    Blockchain(BlockRate secondsPerBlock, ValueRate transactionFeeRate);
    const std::deque<Block *> &getHeads() const;
    const std::vector<Block *> &getOldHeads() const;
    const std::vector<Block *> getHeadsOfHeight(BlockHeight height) const;
    
    void publishBlock(std::unique_ptr<MinedBlock> block);
    
    
    void printBlockchain() const;
    void printHeads() const;
    void printOldHeads() const;
    
    const Block &winningHead() const;
    
    const std::vector<Block *> oldestPublishedHeads() const;
    Block &oldestPublishedHead() const;
    Block &smallestPublishedHead(BlockHeight age) const;
    
    void tick(BlockTime timePassed);
    void advanceToTime(BlockTime time);
    
    inline BlockHeight getMaxHeightPub() const {
        return _maxHeightPub;
    }
    
    inline BlockTime getTime() const {
        return timeInSecs;
    }
    
    inline Value getValueNetworkTotal() const {
        return valueNetworkTotal;
    }
    
    BlockValue expectedBlockSize() const;
    TimeRate chanceToWin(HashRate hashRate) const;
};

#endif /* blockchain_hpp */
