//
//  block.cpp
//  BlockSim
//
//  Created by Harry Kalodner on 5/25/16.
//  Copyright © 2016 Harry Kalodner. All rights reserved.
//

#include "block.hpp"
#include "miner.hpp"

#include <cassert>
#include <iostream>
#include <limits>

constexpr auto timeMax = std::numeric_limits<TimeType>::max();

Block::Block(BlockValue blockReward_) : Block(nullptr, nullptr, BlockTime(0), Value(0), BlockHeight(0), Value(0), Value(0), Value(rawValue(blockReward_))) {}

Block::Block(const Block *parent_, const Miner *miner_, BlockTime timeSeconds_, Value txFees, BlockHeight height_, Value txFeesInChain_, Value valueInChain_, Value blockReward_) : timeBroadcast(timeMax), parent(parent_), miner(miner_), height(height_), timeMined(timeSeconds_), value(txFees + blockReward_), txFeesInChain(txFeesInChain_), valueInChain(valueInChain_), blockReward(blockReward_) {}

Block::Block(const Block *parent_, const Miner *miner_, BlockTime timeSeconds_, Value txFees) :
    Block(parent_, miner_, timeSeconds_, txFees, parent_->height + BlockHeight(1), parent_->txFeesInChain + txFees, parent_->valueInChain + parent_->blockReward + txFees, parent_->nextBlockReward()) {}

void Block::reset(const Block *parent_, const Miner *miner_, BlockTime timeSeconds_, Value txFees) {
    height = parent_->height + BlockHeight(1);
    timeMined = timeSeconds_;
    timeBroadcast = timeMax;
    value = txFees + parent_->nextBlockReward();
    txFeesInChain = txFees + parent_->txFeesInChain;
    valueInChain = txFees + parent_->valueInChain + parent_->nextBlockReward();
    blockReward = parent_->nextBlockReward();
    parent = parent_;
    miner = miner_;
}

Value Block::nextBlockReward() const {
    return blockReward;
}

void Block::broadcast(BlockTime timePub) {
    timeBroadcast = timePub;
}

bool Block::isBroadcast() const {
    return timeBroadcast < timeMax;
}

BlockTime Block::getTimeBroadcast() const {
    return timeBroadcast;
}

std::ostream& operator<< (std::ostream& out, const Block& mc) {
    mc.print(out, true);
    return out;
}

std::vector<const Block *> Block::getChain() const {
    std::vector<const Block *> chain;
    const Block *current = this;
    while (current) {
        chain.push_back(current);
        current = current->parent;
    }
    return chain;
}

void Block::print(std::ostream& os, bool isPublished) const {
    if (height == BlockHeight(0)) {
        os << "[h:0, m:gen]";
        return;
    }
    if (isPublished) {
        os << "{";
    }
    else {
        os << "[";
    }
    
    os << "h:" << height << ", m:" << miner->params.name << ", v:" << value << ", t:" << timeMined;
    
    if (isPublished) {
        os << "}->";
    }
    else {
        os << "]->";
    }
}

bool Block::minedBy(const Miner *miner_) const {
    return miner == miner_;
}
