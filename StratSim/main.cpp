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
#include "multiplicative_weights.hpp"
#include "minerStrategies.h"
#include "logging.h"
#include "game.hpp"
#include "minerGroup.hpp"
#include "mining_style.hpp"
#include "miner_result.hpp"
#include "game_result.hpp"

#include <iostream>

//--more representative of smaller miners, where the chance that you mine the
//next block is ~0 (not to be confused with the strategy selfish mining)

#define NO_SELF_MINING true     //not realistic, but do not force miners to mine on top of their own blocks
#define NOISE_IN_TRANSACTIONS false //miners don't put the max value they can into a block (simulate tx latency)

#define NETWORK_DELAY BlockTime(8)         //network delay in seconds for network to hear about your block
#define EXPECTED_NUMBER_OF_BLOCKS BlockCount(10000)

#define LAMBERT_COEFF 0.13533528323661//coeff for lambert func equil  must be in [0,.2]
//0.13533528323661 = 1/(e^2)

struct RunSettings {
    unsigned int numberOfGames;
    MinerCount totalMiners;
    BlockRate secondsPerBlock;
    BlockCount blockCount;
};

void runStratGame(RunSettings settings, MinerCount fixedDefault, MultiplicativeWeights &stratPool, bool outputWeights);

void runStrackedStratGame(RunSettings settings);
void runSingleStratGame(RunSettings settings, MinerCount fixedDefault);

void runStratGame(RunSettings settings, MinerCount fixedDefault, MultiplicativeWeights &stratPool, bool outputWeights) {
    
    //start running games
    BlockCount totalBlocksMined(0);
    BlockCount blocksInLongestChain(0);
    
    for (unsigned int gameNum = 0; gameNum < settings.numberOfGames; gameNum++) {
        
        GAMEINFOBLOCK(
            GAMEINFO("Current strategy weights:" << std::endl);
            stratPool.printWeights();
        )
        
        
        MinerCount numberRandomMiners(settings.totalMiners - fixedDefault);
        
        std::vector<std::unique_ptr<Miner>> miners;
        HashRate hashRate(1.0/rawCount(settings.totalMiners));
        for (MinerCount i(0); i < settings.totalMiners; i++) {
            auto minerName = std::to_string(rawCount(i));
            MinerParameters parameters {rawCount(i), minerName, hashRate, NETWORK_DELAY, COST_PER_SEC_TO_MINE};
            if (i < numberRandomMiners) {
                const auto &strat = stratPool.pickRandomStrategy();
                miners.push_back(strat.generateMiner(parameters));
            } else {
                miners.push_back(stratPool.defaultStrategy.generateMiner(parameters));
            }
        }
        
        MinerGroup minerGroup(std::move(miners));
        
        GAMEINFO("\n\nGame#: " << gameNum << " The board is set, the pieces are in motion..." << std::endl);
        
        auto result = runGame(minerGroup, settings.blockCount, settings.secondsPerBlock, A);
        GAMEINFO("The game is complete. Calculate the scores:" << std::endl);
        
        if (outputWeights) {
            //print to files the normalized weights of the strategies
            stratPool.writeWeights(gameNum);
        }
        
        //update the stratPool's weights:
        //current version sets the weights to = fraction profits this strat/total profits
        
        stratPool.calculateNewNormalizedWeights(minerGroup, result.second.minerResults);
        totalBlocksMined += result.second.totalBlocksMined;
        blocksInLongestChain += result.second.blocksInLongestChain;
//        std::cout << 100 * double(rawHeight(result.second.blocksInLongestChain)) / double(rawHeight(result.second.totalBlocksMined)) << "% in final chain" << std::endl;
    }
    
    if (outputWeights) {
        stratPool.writeWeights(settings.numberOfGames);
    }
    
//    std::cout << 100 * double(rawHeight(blocksInLongestChain)) / double(rawHeight(totalBlocksMined)) << "% in final chain" << std::endl;
    
    GAMEINFOBLOCK(
        GAMEINFO("Games over. Final strategy weights:\n");
        stratPool.printWeights();
    )
}

void runSingleStratGame(RunSettings settings, MinerCount fixedDefault) {
    using std::placeholders::_1;
    using std::placeholders::_2;
    
    Strategy defaultStrategy = createDefaultStrategy(NO_SELF_MINING, NOISE_IN_TRANSACTIONS);
    MultiplicativeWeights stratPool(std::to_string(rawCount(fixedDefault)), defaultStrategy, .3);
    //initial conditions
    
    //    replace the below with the below below to switch to MW for selfish miners w/ different Betas
    //    for (int i = 0; i < 5; i++) {
    //        stratPool.addStrat({StrategyType::clever_selfish_mining, i * 0.25, NOISE_IN_TRANSACTIONS, NO_SELF_MINING}, 1);
    //    }
    
    StratWeight defaultWeight(1);
    
    for (int i = 0; i < 8; i++) {
        int funcCoeff = static_cast<int>(pow(2, (i + 1)));
        std::function<Value(const Blockchain &, Value)> forkFunc(std::bind(functionForkPercentage, _1, _2, funcCoeff));
        stratPool.addStrat(createFunctionForkStrategy(NO_SELF_MINING, forkFunc, std::to_string(funcCoeff)), defaultWeight);
    }
    stratPool.addStrat(createFunctionForkStrategy(NO_SELF_MINING, std::bind(functionForkLambert, _1, _2, LAMBERT_COEFF), "lambert"), defaultWeight);
    stratPool.addStrat(createUndercutStrategy(NO_SELF_MINING, NOISE_IN_TRANSACTIONS), defaultWeight);
    stratPool.addStrat(createPettyStrategy(NO_SELF_MINING, NOISE_IN_TRANSACTIONS), defaultWeight);
    
    runStratGame(settings, fixedDefault, stratPool, true);
}

void runStrackedStratGame(RunSettings settings) {
    using std::placeholders::_1;
    using std::placeholders::_2;
    
    Strategy defaultStrategy = createDefaultStrategy(NO_SELF_MINING, NOISE_IN_TRANSACTIONS);
    
    MultiplicativeWeights stratPool("stackPlot", defaultStrategy, .1);
    StratWeight defaultWeight(1);
    
    for (int i = -1; i < 7; i++) {
        double funcCoeff = pow(2, -(i + 1));
        std::function<Value(const Blockchain &, Value)> forkFunc(std::bind(functionForkPercentage, _1, _2, funcCoeff));
        stratPool.addStrat(createFunctionForkStrategy(NO_SELF_MINING, forkFunc, std::to_string(funcCoeff)), defaultWeight);
    }
    stratPool.addStrat(createFunctionForkStrategy(NO_SELF_MINING, std::bind(functionForkLambert, _1, _2, LAMBERT_COEFF), "lambert"), defaultWeight);
    stratPool.addStrat(createUndercutStrategy(NO_SELF_MINING, NOISE_IN_TRANSACTIONS), defaultWeight);
    //    stratPool.addStrat({StrategyType::clever_fork, NOISE_IN_TRANSACTIONS, NO_SELF_MINING}, defaultWeight);
    stratPool.addStrat(createPettyStrategy(NO_SELF_MINING, NOISE_IN_TRANSACTIONS), defaultWeight);
    
    std::ofstream defaultOut("stackPlot/default.txt");
    for (MinerCount numberDefaultMiners(0); numberDefaultMiners < settings.totalMiners; numberDefaultMiners++) {
        runStratGame(settings, numberDefaultMiners, stratPool, false);
        stratPool.writeWeights(rawCount(numberDefaultMiners), settings.totalMiners, numberDefaultMiners);
        
        double defaultRatio = static_cast<double>(rawCount(numberDefaultMiners)) / rawCount(settings.totalMiners);
        defaultOut << rawCount(numberDefaultMiners) << " " << defaultRatio << std::endl;
        stratPool.resetWeights();
    }
}

int main(int, const char * []) {
//    for (MinerCount i(17); i < MinerCount(20); i++) {
//        runSingleStratGame(i);
//    }
    runSingleStratGame({20000, MinerCount(20000), SEC_PER_BLOCK, EXPECTED_NUMBER_OF_BLOCKS}, MinerCount(0));
//    runSingleStratGame();
//    runStrackedStratGame();
}
