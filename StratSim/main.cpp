//
//  main.cpp
//  BlockSim
//
//  Created by Harry Kalodner on 6/6/16.
//  Copyright © 2016 Harry Kalodner. All rights reserved.
//

#include "strategy.hpp"
#include "block.hpp"
#include "blockchain.hpp"
#include "minerStrategies.h"
#include "logging.h"
#include "game.hpp"
#include "minerGroup.hpp"
#include "mining_style.hpp"
#include "miner_result.hpp"
#include "game_result.hpp"
#include "typeDefs.hpp"
#include "learning_miner.hpp"
#include "minerImp.hpp"

#include <iostream>
#include <iomanip>
#include <stdlib.h>
#include <sys/stat.h>

//--more representative of smaller miners, where the chance that you mine the
//next block is ~0 (not to be confused with the strategy selfish mining)

#define NO_SELF_MINING true     //not realistic, but do not force miners to mine on top of their own blocks
#define NOISE_IN_TRANSACTIONS false //miners don't put the max value they can into a block (simulate tx latency)

#define NETWORK_DELAY BlockTime(0)         //network delay in seconds for network to hear about your block
#define EXPECTED_NUMBER_OF_BLOCKS BlockCount(10000)

#define LAMBERT_COEFF 0.13533528323661//coeff for lambert func equil  must be in [0,.2]
//0.13533528323661 = 1/(e^2)

struct RunSettings {
    unsigned int numberOfGames;
    MinerCount totalMiners;
    BlockRate secondsPerBlock;
    BlockCount blockCount;
};

void runStratGame(RunSettings settings, MinerCount fixedDefault, std::vector<Strategy> &strategies, Strategy defaultStrategy, std::string folderPrefix);

void runSingleStratGame(RunSettings settings, MinerCount fixedDefault, std::string folderPrefix);

void runStratGame(RunSettings settings, MinerCount fixedDefault, std::vector<Strategy> &strategies, Strategy defaultStrategy, std::string folderPrefix) {
    
    //start running games
    BlockCount totalBlocksMined(0);
    BlockCount blocksInLongestChain(0);
    
    std::string resultFolder = "";
    
    if (folderPrefix.length() > 0) {
        resultFolder += folderPrefix + "-";
    }
    
    resultFolder += std::to_string(rawCount(fixedDefault));
    
    char final [256];
    sprintf (final, "./%s", resultFolder.c_str());
    mkdir(final,0775);
    
    std::vector<std::unique_ptr<Miner>> miners;
    std::vector<LearningMiner *> learningMiners;
    
    HashRate hashRate(1.0/rawCount(settings.totalMiners));
    MinerCount numberRandomMiners(settings.totalMiners - fixedDefault);
    
    for (MinerCount i(0); i < settings.totalMiners; i++) {
        auto minerName = std::to_string(rawCount(i));
        MinerParameters parameters {rawCount(i), minerName, hashRate, NETWORK_DELAY, COST_PER_SEC_TO_MINE};
        
        if (i < numberRandomMiners) {
            auto learningMiner = std::make_unique<LearningMiner>(parameters, strategies);
            learningMiners.push_back(learningMiner.get());
            miners.push_back(std::unique_ptr<Miner>(std::move(learningMiner)));
        } else {
            miners.push_back(std::make_unique<Miner>(parameters, defaultStrategy.createImp()));
        }
    }
    
    MinerGroup minerGroup(learningMiners, std::move(miners), strategies, resultFolder);
    
//    double phi = std::sqrt(strategies.size() * std::log(strategies.size())) / std::sqrt(settings.numberOfGames);
//    double phi = std::sqrt(strategies.size() * std::log(strategies.size())) / std::sqrt(settings.numberOfGames / 100);
    
    double phi = .01;
    
    for (unsigned int gameNum = 0; gameNum < settings.numberOfGames; gameNum++) {
//        double n = gameNum;
//        double nMax = settings.numberOfGames;
//        double phi = std::pow(.9, (n / nMax) * 30.0);

        minerGroup.writeWeights(gameNum);
        minerGroup.updateProbabilities(phi); // Step 1
        minerGroup.resetAndpickNewStrategies(); // Step 2
        
        GAMEINFO("\n\nGame#: " << gameNum << " The board is set, the pieces are in motion..." << std::endl);
        
        auto result = runGame(minerGroup, settings.blockCount, settings.secondsPerBlock, A);
        
        GAMEINFO("The game is complete. Calculate the scores:" << std::endl);
        
        Value maxProfit = (A * (EXPECTED_NUMBER_OF_BLOCKS * settings.secondsPerBlock) - result.second.moneyLeftAtEnd) / Value(rawCount(settings.totalMiners) / 4);
        
        // Steps 3, 4, 5
        minerGroup.updateWeights(result.second, maxProfit, phi);
        
        totalBlocksMined += result.second.totalBlocksMined;
        blocksInLongestChain += result.second.blocksInLongestChain;
    }
    
    minerGroup.writeWeights(settings.numberOfGames);
    
    //    std::cout << 100 * double(rawHeight(blocksInLongestChain)) / double(rawHeight(totalBlocksMined)) << "% in final chain" << std::endl;
    
    GAMEINFOBLOCK(
                  GAMEINFO("Games over. Final strategy weights:\n");
                  stratPool.printWeights();
                  )
}

void runSingleStratGame(RunSettings settings, MinerCount fixedDefault, std::string folderPrefix) {
    using std::placeholders::_1;
    using std::placeholders::_2;
    
    std::vector<Strategy> strategies;
    
    Strategy defaultStrategy = createDefaultStrategy(NO_SELF_MINING, NOISE_IN_TRANSACTIONS);

    strategies.push_back(createPettyStrategy(NO_SELF_MINING, NOISE_IN_TRANSACTIONS));
    for (int i = -1; i < 3; i++) {
        int funcCoeff = static_cast<int>(pow(2, (i + 1)));
        std::function<Value(const Blockchain &, Value)> forkFunc(std::bind(functionForkPercentage, _1, _2, funcCoeff));
        strategies.push_back(createFunctionForkStrategy(NO_SELF_MINING, forkFunc, std::to_string(funcCoeff)));
    }
//  strategies.push_back(createFunctionForkStrategy(NO_SELF_MINING, std::bind(functionForkLambert, _1, _2, LAMBERT_COEFF), "lambert"));
    strategies.push_back(createLazyForkStrategy(NO_SELF_MINING));
    
    
    runStratGame(settings, fixedDefault, strategies, defaultStrategy, folderPrefix);
}

int main(int, const char * []) {
    runSingleStratGame({400000, MinerCount(200), SEC_PER_BLOCK, EXPECTED_NUMBER_OF_BLOCKS}, MinerCount(0),"test");
    
}
