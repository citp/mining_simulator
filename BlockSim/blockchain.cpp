//
//  blockchain.cpp
//  BlockSim
//
//  Created by Harry Kalodner on 5/25/16.
//  Copyright © 2016 Harry Kalodner. All rights reserved.
//

#include "blockchain.hpp"
#include "utils.hpp"
#include "block.hpp"

#include <iostream>
#include <algorithm>
#include <cassert>
#include <cmath>
#include <limits>
#include <unordered_set>

#define HEAD_AGE BlockHeight(3)          //age of heads kept in the heads queue before moving them to the oldHeads queue

Blockchain::Blockchain(BlockchainSettings blockchainSettings) :
    genesis(std::make_unique<GenesisBlock>(blockchainSettings.blockReward)),
    valueNetworkTotal(0),
    timeInSecs(0),
    secondsPerBlock(blockchainSettings.secondsPerBlock),
    transactionFeeRate(blockchainSettings.transactionFeeRate),
    _maxHeightPub(0)
{
    genesis->publish(BlockTime(0));
    heads.push_back(genesis.get());
    _smallestBlocks.push_back({genesis.get()});
    std::vector<Block *> newBlocks;
    newBlocks.push_back(genesis.get());
    _recentBlocksOfHeight.push_front(newBlocks);
    assert(heads[0] != nullptr);
}

const std::deque<Block *> &Blockchain::getHeads() const {
    return heads;
}

const std::vector<Block *> &Blockchain::getOldHeads() const {
    return oldHeads;
}

void Blockchain::newBlockAdded(Block *block) {
    assert(block->height <= _maxHeightPub + BlockHeight(1));
    if (block->height > _maxHeightPub) {
        _maxHeightPub = block->height;
        _smallestBlocks.push_back({block});
        std::vector<Block *> newBlocks;
        newBlocks.push_back(block);
        _recentBlocksOfHeight.push_front(newBlocks);
        if (_recentBlocksOfHeight.size() > rawHeight(HEAD_AGE)) {
            _recentBlocksOfHeight.pop_back();
        }
    } else {
        HeightType height = rawHeight(block->height);
        if (valueEquals(block->value, _smallestBlocks[height][0]->value)) {
            _smallestBlocks[height].push_back(block);
        } else if (block->value < _smallestBlocks[height][0]->value) {
            _smallestBlocks[height].clear();
            _smallestBlocks[height].push_back(block);
        }
        
        if (block->height >= _maxHeightPub - HEAD_AGE) {
            _recentBlocksOfHeight[rawHeight(_maxHeightPub - block->height)].push_back(block);
        }
    }
}

void Blockchain::publishBlock(std::unique_ptr<MinedBlock> block) {
    assert(block);
    
    block->publish(timeInSecs);
    
    newBlockAdded(block.get());
    
    size_t headsSize = heads.size();
    size_t oldHeadsSize = oldHeads.size();
    
    bool replacedHead = false;
    
    heads.push_back(block.get());
    bool startAging = getMaxHeightPub() > HEAD_AGE;
    for(auto it = heads.begin(); it != heads.end();)
    {
        auto current = *it;
        if (&block->parent == current) {
            it = heads.erase(it);
            replacedHead = true;
        } else if (startAging && current->height < (getMaxHeightPub() - HEAD_AGE)) {
            oldHeads.push_back(*it);
            it = heads.erase(it);
        } else {
            ++it;
        }
    }
    
    assert(headsSize + oldHeadsSize + (replacedHead ? 0 : 1) == heads.size() + oldHeads.size());
    assert(heads.size() > 0);
    
    block->parent.addChild(std::move(block));

    for (const auto &head : heads) {
        assert(!startAging || head->height >= (getMaxHeightPub() - HEAD_AGE));
    }
}


// Following comment kept for the records. Now the same is acheived by keeping a set of already printed blocks here

//what's the purpose of this? I'll tell you: all the wasPrinted and clear printing stuff is here so when you
//print the block chain, you see all the heads, but once they converge onto a single path, it's only
//printed once. (otherwise you'd head num_heads*blockchain.length prints)

void Blockchain::printBlockchain() const {
    std::unordered_set<const Block *> printedBlocks;
    for (auto current : heads) {
        auto chain = current->getChain();
        for (const Block &block : chain) {
            if (printedBlocks.find(&block) == end(printedBlocks)) {
                std::cout << block;
                printedBlocks.insert(&block);
            } else {
                break;
            }
        }
        std::cout << std::endl;
    }
}

void Blockchain::printHeads() const {
    std::cout << "heads:" << std::endl;
    for (auto current : heads) {
        std::cout << *current << std::endl;
    }
    std::cout << "end heads." << std::endl;
}

void Blockchain::printOldHeads() const {
    std::cout << "Old heads:" << std::endl;
    for (auto &current : oldHeads) {
        std::cout << *current << std::endl;
    }
    std::cout << "end old heads." << std::endl;
}

const std::vector<Block *> Blockchain::getHeadsOfHeight(BlockHeight height) const {
    std::vector<Block *> blocks;
    for (Block *head : getHeads()) {
        if (head->height < height) {
            continue;
        }
        blocks.push_back(&head->getAncestorOfHeight(height));
    }
    assert(blocks.size() != 0);
    return blocks;
}

const std::vector<Block *> Blockchain::oldestPublishedHeads(BlockHeight height) const {
    auto blocks = getHeadsOfHeight(getMaxHeightPub() - height);
    BlockTime timePublished(std::numeric_limits<BlockTime>::max());
    for (const auto &block : blocks) {
        if (block->getTimePublished() < timePublished) {
            timePublished = block->getTimePublished();
        }
    }
    
    std::vector<Block *> possiblities;
    for (const auto &block : blocks) {
        if (block->getTimePublished() <= timePublished) {
            possiblities.push_back(block);
        }
    }
    
    assert(possiblities.size() > 0);
    
    return possiblities;
}

Block &Blockchain::oldestPublishedHead(BlockHeight height) const {
    auto possiblities = oldestPublishedHeads(height);
    return *possiblities[selectRandomIndex(possiblities.size())];
}

Block &Blockchain::most(BlockHeight age, const Miner &miner) const {
    assert(age < HEAD_AGE);
    for (auto &block : _recentBlocksOfHeight[rawHeight(age)]) {
        if (block->minedBy(miner)) {
            return *block;
        }
    }
    
    return smallestHead(age);
}

Block &Blockchain::oldest(BlockHeight age, const Miner &miner) const {
    assert(age < HEAD_AGE);
    for (auto &block : _recentBlocksOfHeight[rawHeight(age)]) {
        if (block->minedBy(miner)) {
            return *block;
        }
    }
    
    return oldestPublishedHead(age);
}


Block &Blockchain::smallestHead(BlockHeight age) const {
    size_t height = rawHeight(getMaxHeightPub() - age);
    Block *block = _smallestBlocks[height][selectRandomIndex(_smallestBlocks[height].size())];
    return *block;
}

const Block &Blockchain::winningHead() const {
    auto possibleWinners = getHeadsOfHeight(getMaxHeightPub());
    return *largestBlock(possibleWinners);
}

void Blockchain::tick(BlockTime timePassed) {
    timeInSecs += timePassed;
    valueNetworkTotal += transactionFeeRate * timePassed;
}

void Blockchain::advanceToTime(BlockTime time) {
    assert(time >= timeInSecs);
    BlockTime difference = time - timeInSecs;
    tick(difference);
}

BlockValue Blockchain::expectedBlockSize() const {
    return transactionFeeRate * secondsPerBlock;
}

TimeRate Blockchain::chanceToWin(HashRate hashRate) const {
    return hashRate / secondsPerBlock;
}

Value Blockchain::gap(BlockHeight i) const {
    return rem(smallestHead(i + BlockHeight(1))) - rem(smallestHead(i));
}

Value Blockchain::rem(const Block &block) const {
    return valueNetworkTotal - block.txFeesInChain;
}
