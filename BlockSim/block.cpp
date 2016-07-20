//
//  block.cpp
//  BlockSim
//
//  Created by Harry Kalodner on 5/25/16.
//  Copyright © 2016 Harry Kalodner. All rights reserved.
//

#include "block.hpp"
#include "miner.hpp"
#include "utils.hpp"

#include <cassert>
#include <cmath>
#include <sstream>
#include <limits>

#define B 0 //50.0//(5.0)   //block reward

Block::~Block() = default;

GenesisBlock::GenesisBlock() : Block(BlockTime(0), Value(0), BlockHeight(0), Value(0), Value(0)) {}

Block::Block(BlockTime timeSeconds_, Value value_, BlockHeight height_, Value valueInChain_, Value valueCreateInChain_) : height(height_), timeMined(timeSeconds_), value(value_), valueInChain(valueInChain_), valueCreatedInChain(valueCreateInChain_), nextBlockReward(B) {}

MinedBlock::MinedBlock(Block &parent_, const Miner &miner_, BlockTime timeSeconds_, Value value_) :
    Block(timeSeconds_, value_, parent_.height + BlockHeight(1), parent_.valueInChain + value_, parent_.valueCreatedInChain + parent_.nextBlockReward),
    parent(parent_), miner(miner_) {}

void Block::addChild(std::unique_ptr<Block> block) {
    
    if (_smallestChildren.empty()) {
        _smallestChildren.push_back(block.get());
    } else {
        if (valueEquals(block->value, _smallestChildren[0]->value)) {
            _smallestChildren.push_back(block.get());
        } else if (block->value < _smallestChildren[0]->value) {
            _smallestChildren.clear();
            _smallestChildren.push_back(block.get());
        }
    }
    
    children.push_back(std::move(block));
}

Block *Block::smallestChild() const {
    if (_smallestChildren.empty()) {
        return nullptr;
    } else {
        return _smallestChildren[selectRandomIndex(_smallestChildren.size())];
    }
}

void Block::publish(BlockTime timePub) {
    timePublished = timePub;
}

BlockTime Block::getTimePublished() const {
    return timePublished;
}

std::ostream& operator<< (std::ostream& out, const Block& mc) {
    mc.print(out, true);
    return out;
}

std::vector<std::reference_wrapper<const Block>> Block::getChain() const {
    std::vector<std::reference_wrapper<const Block>> chain;
    const Block *current = this;
    while (current) {
        chain.push_back(*current);
        auto minedCurrent = dynamic_cast<const MinedBlock *>(current);
        if (minedCurrent) {
            current = &minedCurrent->parent;
        } else {
            break;
        }
    }
    return chain;
}

Block &Block::getAncestorOfHeight(BlockHeight blockHeight) {
    assert(blockHeight >= BlockHeight(0));
    Block *current = this;
    while (current->height > blockHeight) {
        auto minedCurrent = dynamic_cast<MinedBlock *>(current);
        assert(minedCurrent);
        current = &minedCurrent->parent;
    }
    assert(current);
    assert(current->height == blockHeight);
    return *current;
}

void GenesisBlock::print(std::ostream& os, bool) const {
    os << "[h:0, m:gen]";
}

void MinedBlock::print(std::ostream& os, bool isPublished) const {
    if (isPublished) {
        os << "{";
    }
    else {
        os << "[";
    }
    
    os << "h:" << height << ", m:" << miner.params.name << ", v:" << value << ", t:" << timeMined;
    
    if (isPublished) {
        os << "}->";
    }
    else {
        os << "]->";
    }
}

bool MinedBlock::minedBy(const Miner &miner_) const {
    return &miner == &miner_;
}

bool GenesisBlock::minedBy(const Miner &) const {
    return false;
}
