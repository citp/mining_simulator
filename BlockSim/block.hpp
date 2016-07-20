//
//  block.hpp
//  BlockSim
//
//  Created by Harry Kalodner on 5/25/16.
//  Copyright © 2016 Harry Kalodner. All rights reserved.
//

#ifndef block_hpp
#define block_hpp

#include "typeDefs.hpp"

#include <vector>
#include <iterator>

class Miner;

class Block {
    std::vector< std::unique_ptr<Block> > children;
    BlockTime timePublished;
    
    std::vector<Block *> _smallestChildren;
public:
    const BlockHeight height;
    const BlockTime timeMined;
    const Value value;
    const Value valueInChain;
    const Value valueCreatedInChain;
    const Value nextBlockReward;
    
    Block();
    Block(BlockTime timeSeconds, Value value, BlockHeight height, Value valueInChain, Value valueCreateInChain);
    
    Block (const Block &block) = delete;
    virtual ~Block();
    
    Block *smallestChild() const;
    
    void publish(BlockTime timePub);
    BlockTime getTimePublished() const;
    
    std::vector<std::reference_wrapper<const Block>> getChain() const;
    
    Block &getAncestorOfHeight(BlockHeight height);
    
    void addChild(std::unique_ptr<Block> block);
    
    virtual bool minedBy(const Miner &miner) const = 0;
    virtual void print(std::ostream& where, bool isPublished) const = 0;
};

std::ostream& operator<< (std::ostream& out, const Block& mc);

class MinedBlock : public Block {
private:
    bool minedBy(const Miner &miner) const override;
    
    void print(std::ostream& where, bool isPublished) const override;
public:
    Block &parent;
    const Miner &miner;
    MinedBlock(Block &parent, const Miner &miner, BlockTime timeSeconds, Value value);
};

class GenesisBlock : public Block {
    bool minedBy(const Miner &miner) const override;
    
    void print(std::ostream& where, bool isPublished) const override;
public:
    GenesisBlock();
};

#endif /* block_hpp */
