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
#include "multiplicative_weights_learning_model.hpp"
#include "exp3_learning_model.hpp"

#include <iostream>
#include <iomanip>
#include <stdlib.h>
#include <sys/stat.h>

//--more representative of smaller miners, where the chance that you mine the
//next block is ~0 (not to be confused with the strategy selfish mining)

#define ATOMIC false     //not realistic, but do not force miners to mine on top of their own blocks
#define NOISE_IN_TRANSACTIONS false //miners don't put the max value they can into a block (simulate tx latency)

#define NETWORK_DELAY BlockTime(0)         //network delay in seconds for network to hear about your block
#define EXPECTED_NUMBER_OF_BLOCKS BlockCount(10000)

#define LAMBERT_COEFF 0.13533528323661//coeff for lambert func equil  must be in [0,.2]
//0.13533528323661 = 1/(e^2)

#define B BlockValue(3.125) // Block reward
//#define TOTAL_BLOCK_VALUE BlockValue(15.625)
#define TOTAL_BLOCK_VALUE BlockValue(13)

#define SEC_PER_BLOCK BlockRate(600)     //mean time in seconds to find a block

//#define B BlockValue(3.125) // Block reward
#define A (TOTAL_BLOCK_VALUE - B)/SEC_PER_BLOCK  //rate transactions come in


struct RunSettings {
    unsigned int numberOfGames;
    MinerCount totalMiners;
    MinerCount fixedDefault;
    GameSettings gameSettings;
    std::string folderPrefix;
};

void runStratGame(RunSettings settings, std::vector<Strategy> &strategies, Strategy defaultStrategy);

void runSingleStratGame(RunSettings settings);

void runStratGame(RunSettings settings, std::vector<Strategy> &strategies, Strategy defaultStrategy) {
    
    //start running games
    BlockCount totalBlocksMined(0);
    BlockCount blocksInLongestChain(0);
    
    std::string resultFolder = "";
    
    if (settings.folderPrefix.length() > 0) {
        resultFolder += settings.folderPrefix + "-";
    }
    
    resultFolder += std::to_string(rawCount(settings.fixedDefault));
    
    char final [256];
    sprintf (final, "./%s", resultFolder.c_str());
    mkdir(final,0775);
    
    std::vector<std::unique_ptr<Miner>> miners;
    std::vector<LearningMiner *> learningMiners;
    
    HashRate hashRate(1.0/rawCount(settings.totalMiners));
    MinerCount numberRandomMiners(settings.totalMiners - settings.fixedDefault);
    
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
    
//    LearningModel *learningModel = new MultiplicativeWeightsLearningModel(strategies, learningMiners, resultFolder);
    LearningModel *learningModel = new Exp3LearningModel(strategies, learningMiners, resultFolder);
    
    MinerGroup minerGroup(std::move(miners));
    
//    double phi = std::sqrt(strategies.size() * std::log(strategies.size())) / std::sqrt(settings.numberOfGames);
//    double phi = std::sqrt(strategies.size() * std::log(strategies.size())) / std::sqrt(settings.numberOfGames / 100);
    
    double phi = .1;
    
    for (unsigned int gameNum = 0; gameNum < settings.numberOfGames; gameNum++) {
//        double n = gameNum;
//        double nMax = settings.numberOfGames;
//        double phi = std::pow(.9, (n / nMax) * 30.0);
        learningModel->writeWeights(gameNum);
        learningModel->pickNewStrategies(phi);
        GAMEINFO("\n\nGame#: " << gameNum << " The board is set, the pieces are in motion..." << std::endl);
        
        auto result = runGame(minerGroup, settings.gameSettings);
        
        GAMEINFO("The game is complete. Calculate the scores:" << std::endl);
        
        Value maxProfit = (A * (EXPECTED_NUMBER_OF_BLOCKS * settings.gameSettings.blockchainSettings.secondsPerBlock) - result.second.moneyLeftAtEnd) / Value(rawCount(settings.totalMiners) / 4);
        
        // Steps 3, 4, 5
        learningModel->updateWeights(result.second, maxProfit, phi);
        
        totalBlocksMined += result.second.totalBlocksMined;
        blocksInLongestChain += result.second.blocksInLongestChain;
        
//        BlockCount staleBlocks(result.second.totalBlocksMined - result.second.blocksInLongestChain);
        
        
//        std::cout << result.second.moneyInLongestChain << " in chain and " <<
//        result.second.moneyLeftAtEnd << " left with " << 100 * double(rawCount(staleBlocks)) / double(rawCount(result.second.totalBlocksMined)) << "% orphan rate" <<  std::endl;
    }
    learningModel->writeWeights(settings.numberOfGames);
    
    delete learningModel;
//    std::cout << 100 * double(rawCount(blocksInLongestChain)) / double(rawCount(totalBlocksMined)) << "% in final chain" << std::endl;
    
    GAMEINFOBLOCK(
                  GAMEINFO("Games over. Final strategy weights:\n");
                  stratPool.printWeights();
                  )
}

void runSingleStratGame(RunSettings settings) {
    using std::placeholders::_1;
    using std::placeholders::_2;
    
    std::vector<Strategy> strategies;
    
    Strategy defaultStrategy = createDefaultStrategy(ATOMIC, NOISE_IN_TRANSACTIONS);

    strategies.push_back(createPettyStrategy(ATOMIC, NOISE_IN_TRANSACTIONS));
    for (int i = -1; i < 3; i++) {
        int funcCoeff = static_cast<int>(pow(2, (i + 1)));
        std::function<Value(const Blockchain &, Value)> forkFunc(std::bind(functionForkPercentage, _1, _2, funcCoeff));
        strategies.push_back(createFunctionForkStrategy(ATOMIC, forkFunc, std::to_string(funcCoeff)));
    }
//  strategies.push_back(createFunctionForkStrategy(NO_SELF_MINING, std::bind(functionForkLambert, _1, _2, LAMBERT_COEFF), "lambert"));
    strategies.push_back(createLazyForkStrategy(ATOMIC));
    
    
    runStratGame(settings, strategies, defaultStrategy);
}

int main(int, const char * []) {
    
    BlockchainSettings blockchainSettings = {SEC_PER_BLOCK, A, B};
    GameSettings gameSettings = {EXPECTED_NUMBER_OF_BLOCKS, blockchainSettings};
    
//    for (MinerCount i(440); i < MinerCount(1001); i += MinerCount(10)) {
//        RunSettings runSettings = {20000, MinerCount(1000), i, gameSettings, "mult"};
//        runSingleStratGame(runSettings);
//    }
    
//    runSingleStratGame({10000000, MinerCount(100), SEC_PER_BLOCK, EXPECTED_NUMBER_OF_BLOCKS}, MinerCount(31),"test");
    RunSettings runSettings = {100000, MinerCount(50), MinerCount(0), gameSettings, "reward2"};
    runSingleStratGame(runSettings);
    
}
