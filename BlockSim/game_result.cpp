//
//  game_result.cpp
//  BlockSim
//
//  Created by Harry Kalodner on 7/1/16.
//  Copyright © 2016 Harry Kalodner. All rights reserved.
//

#include "game_result.hpp"

#include "miner_result.hpp"

GameResult::GameResult(std::map<const Miner *, MinerResult> minerResults_, BlockCount totalBlocksMined_, BlockCount blocksInLongestChain_, Value moneyLeftAtEnd_, Value moneyInLongestChain_) : minerResults(minerResults_), totalBlocksMined(totalBlocksMined_), blocksInLongestChain(blocksInLongestChain_), moneyLeftAtEnd(moneyLeftAtEnd_), moneyInLongestChain(moneyInLongestChain_) {}
