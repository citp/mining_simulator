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
#include "blockchain_settings.hpp"

#include <cassert>
#include <unordered_set>
#include <iostream>

Blockchain::Blockchain(BlockchainSettings blockchainSettings) :
    valueNetworkTotal(0),
    timeInSecs(0),
    secondsPerBlock(blockchainSettings.secondsPerBlock),
    transactionFeeRate(blockchainSettings.transactionFeeRate),
    _maxHeightPub(0)
{
    _blocks.reserve(rawCount(blockchainSettings.numberOfBlocks) * 2);
    _blocksIndex.resize(rawCount(blockchainSettings.numberOfBlocks) * 2);
    _smallestBlocks.resize(rawCount(blockchainSettings.numberOfBlocks) * 2);
    reset(blockchainSettings);
}

std::unique_ptr<Block> Blockchain::createBlock(const Block *parent, const Miner *miner, Value value) {
    Value txFees = value - parent->nextBlockReward();
    if (_oldBlocks.size() == 0) {
        return std::make_unique<Block>(parent, miner, getTime(), txFees);
    }
    
    auto block = std::move(_oldBlocks.back());
    _oldBlocks.pop_back();
    block->reset(parent, miner, getTime(), txFees);
    return block;
}

void Blockchain::reset(BlockchainSettings blockchainSettings) {
    valueNetworkTotal = 0;
    timeInSecs = BlockTime(0);
    secondsPerBlock = blockchainSettings.secondsPerBlock;
    transactionFeeRate = blockchainSettings.transactionFeeRate;
    _maxHeightPub = BlockHeight(0);
    _oldBlocks.reserve(_oldBlocks.size() + _blocks.size());
    for (auto &block : _blocks) {
        _oldBlocks.push_back(std::move(block));
    }
    _blocks.clear();
    _smallestBlocks[0].clear();
    _blocksIndex[0].clear();
    auto genesis = std::make_unique<Block>(blockchainSettings.blockReward);
    _smallestBlocks[0].push_back(genesis.get());
    _blocksIndex[0].push_back(_blocks.size());
    _blocks.push_back(std::move(genesis));
}

void Blockchain::publishBlock(std::unique_ptr<Block> block) {
    assert(block);
    assert(block->height <= _maxHeightPub + BlockHeight(1));
    
    HeightType height = rawHeight(block->height);
    
    if (block->height > _maxHeightPub) {
        _blocksIndex[height].clear();
        _smallestBlocks[height].clear();
        _smallestBlocks[height].push_back(block.get());
        _maxHeightPub = block->height;
    } else {
        std::vector<Block *> &smallestVector = _smallestBlocks[height];
        
        if (block->value < smallestVector.front()->value) {
            smallestVector.clear();
            smallestVector.push_back(block.get());
        } else if (block->value == smallestVector.front()->value) {
            smallestVector.push_back(block.get());
        }
    }

    _blocksIndex[height].push_back(_blocks.size());
    _blocks.push_back(std::move(block));
}

BlockCount Blockchain::blocksOfHeight(BlockHeight height) const {
    return BlockCount(_blocksIndex[rawHeight(height)].size());
}

const std::vector<Block *> Blockchain::oldestPublishedHeads(BlockHeight height) const {
    BlockTime minTimePublished(std::numeric_limits<BlockTime>::max());
    for (size_t index : _blocksIndex[rawHeight(height)]) {
        minTimePublished = std::min(_blocks[index]->getTimeBroadcast(), minTimePublished);
    }
    
    std::vector<Block *> possiblities;
    for (size_t index : _blocksIndex[rawHeight(height)]) {
        if (_blocks[index]->getTimeBroadcast() == minTimePublished) {
            possiblities.push_back(_blocks[index].get());
        }
    }
    assert(possiblities.size() > 0);
    return possiblities;
}

Block &Blockchain::oldestPublishedHead(BlockHeight height) const {
    auto possiblities = oldestPublishedHeads(height);
    return *possiblities[selectRandomIndex(possiblities.size())];
}

Block &Blockchain::smallestHead(BlockHeight height) const {
    auto &smallestBlocks = _smallestBlocks[rawHeight(height)];
    size_t index = selectRandomIndex(smallestBlocks.size());
    Block *block = smallestBlocks[index];
    return *block;
}

const Block &Blockchain::winningHead() const {
    Value largestValue(0);
    for (size_t index : _blocksIndex[rawHeight(getMaxHeightPub())]) {
        largestValue = std::max(largestValue, _blocks[index]->valueInChain);
    }
    
    std::vector<Block *> possiblities;
    for (size_t index : _blocksIndex[rawHeight(getMaxHeightPub())]) {
        if (_blocks[index]->valueInChain == largestValue) {
            possiblities.push_back(_blocks[index].get());
        }
    }
    std::uniform_int_distribution<std::size_t> vectorDis(0, possiblities.size() - 1);
    return *possiblities[selectRandomIndex(possiblities.size())];
}

void Blockchain::advanceToTime(BlockTime time) {
    assert(time >= timeInSecs);
    valueNetworkTotal += transactionFeeRate * (time - timeInSecs);
    timeInSecs = time;
}

BlockValue Blockchain::expectedBlockSize() const {
    return transactionFeeRate * secondsPerBlock + BlockValue(_smallestBlocks[rawHeight(getMaxHeightPub())].front()->nextBlockReward());
}

TimeRate Blockchain::chanceToWin(HashRate hashRate) const {
    return hashRate / secondsPerBlock;
}

Block *Blockchain::blockByMinerAtHeight(BlockHeight height, const Miner &miner) const {
    for (size_t index : _blocksIndex[rawHeight(height)]) {
        if (_blocks[index]->minedBy(&miner)) {
            return _blocks[index].get();
        }
    }
    return nullptr;
}

Block &Blockchain::most(BlockHeight height, const Miner &miner) const {
    Block *block = blockByMinerAtHeight(height, miner);
    if (block) {
        return *block;
    }
    
    return smallestHead(height);
}

Block &Blockchain::oldest(BlockHeight height, const Miner &miner) const {
    Block *block = blockByMinerAtHeight(height, miner);
    if (block) {
        return *block;
    }
    
    return oldestPublishedHead(height);
}

Value Blockchain::gap(BlockHeight height) const {
    return rem(smallestHead(height - BlockHeight(1))) - rem(smallestHead(height));
}

Value Blockchain::rem(const Block &block) const {
    return valueNetworkTotal - block.txFeesInChain;
}

