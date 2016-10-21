//
//  game.cpp
//  BlockSim
//
//  Created by Harry Kalodner on 6/6/16.
//  Copyright © 2016 Harry Kalodner. All rights reserved.
//

#include "game.hpp"
#include "blockchain.hpp"
#include "block.hpp"
#include "miner.hpp"
#include "utils.hpp"
#include "logging.h"
#include "minerGroup.hpp"
#include "miner_result.hpp"
#include "game_result.hpp"


#include <cassert>
#include <iostream>

std::pair<std::unique_ptr<Blockchain>, GameResult> runGame(MinerGroup &minerGroup, BlockCount numberOfBlocks, BlockRate secondsPerBlock, ValueRate transactionFeeRate) {
    //set up the blockchain
    
    auto blockchain = std::make_unique<Blockchain>(secondsPerBlock, transactionFeeRate);
    
    minerGroup.initialize(*blockchain);
    minerGroup.resetOrder();
    
    GAMEINFO("Players:" << std::endl << minerGroup);
    
    //mining loop
    
    BlockTime totalSeconds = numberOfBlocks * secondsPerBlock;
    
    while (blockchain->getTime() < totalSeconds) {
        
        BlockTime nextTime = minerGroup.nextEventTime();
        
        assert(blockchain->getTime() <= nextTime);
        
        blockchain->advanceToTime(nextTime);
        
        assert(blockchain->getTime() == nextTime);
        
        //steps through in second intervals
        //on each step each miner gets a turn
        COMMENTARY("Round " << blockchain->getTime() << " of the game..." << std::endl);
        
        minerGroup.nextMineRound(*blockchain);
        
        COMMENTARY("Publish phase:" << std::endl);
        
        minerGroup.nextPublishRound(*blockchain);
        
        COMMENTARY("Round " << blockchain->getTime() << " over. Current blockchain:" << std::endl);
        COMMENTARYBLOCK (
            blockchain->printBlockchain();
            blockchain->printHeads();
            blockchain->printOldHeads();
        )
    }
    
    std::map<const Miner *, MinerResult> minerResults;
    for (const auto &miner : minerGroup.miners) {
        minerResults[miner.get()] = MinerResult();
    }
    
    auto winningChain = blockchain->winningHead().getChain();
    int parentCount = 0;
    for (auto block : winningChain) {
        
        auto minedBlock = dynamic_cast<const MinedBlock *>(&block.get());
        if (minedBlock) {
            if (minedBlock->parent.minedBy(minedBlock->miner)) {
                parentCount++;
            }
            minerResults[&minedBlock->miner].addBlock(minedBlock);
        }
    }
    
//    std::cout << parentCount << " block mined over parent" << std::endl;
    
    
    //calculate the score at the end
    BlockCount totalBlocks(0);
    BlockCount finalBlocks(0);
    
    for (const auto &miner : minerGroup.miners) {
        GAMEINFO(*miner << " earned:" << minerResults[miner.get()].totalProfit << " mined " << miner->getBlocksMinedTotal() <<" total, of which " << minerResults[miner.get()].blocksInWinningChain << " made it into the final chain" << std::endl);
        totalBlocks += miner->getBlocksMinedTotal();
        finalBlocks += minerResults[miner.get()].blocksInWinningChain;
    }
    
    Value moneyLeftAtEnd = getRem(blockchain->getTotalFees(), winningChain[0]);
    
    GameResult result(minerResults, totalBlocks, finalBlocks, moneyLeftAtEnd);
    
    GAMEINFO("Total blocks mined:" << totalBlocks << " with " << finalBlocks << " making it into the final chain" << std::endl);
    
    return std::make_pair(std::move(blockchain), result);
}
