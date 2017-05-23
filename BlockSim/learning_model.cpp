//
//  learning_model.cpp
//  BlockSim
//
//  Created by Harry Kalodner on 10/24/16.
//  Copyright © 2016 Harry Kalodner. All rights reserved.
//

#include "learning_model.hpp"
#include "learning_strategy.hpp"
#include "strategy.hpp"
#include "publishing_strategy.hpp"
#include "miner.hpp"
#include "utils.hpp"
#include "game_result.hpp"
#include "miner_result.hpp"
#include "block.hpp"

#include <algorithm>
#include <sstream>
#include <fstream>
#include <random>
#include <assert.h>
#include <iostream>
#include <sys/stat.h>

LearningModel::LearningModel(std::vector<std::unique_ptr<LearningStrategy>> &learningStrategies_, size_t minerCount_, std::string resultFolder) : learningStrategies(std::move(learningStrategies_)), stratCount(learningStrategies.size()), minerCount(minerCount_) {
    
    assert(learningStrategies.size() > 0);
    
    char final [256];
    sprintf (final, "./%s", resultFolder.c_str());
    mkdir(final,0775);
    
    outputStreams.reserve(stratCount);
    chosenStrats.resize(minerCount);
    
    StratWeight totalWeight(0);
    for (auto &learningStrategy : learningStrategies) {
        totalWeight += learningStrategy->weight;
    }
    
    for (size_t strategyIndex = 0; strategyIndex < learningStrategies.size(); strategyIndex++) {
        std::stringstream ss;
        ss << resultFolder << "/index-" << strategyIndex << "-" << learningStrategies[strategyIndex]->strat->name << ".txt";
        outputStreams.push_back(std::ofstream(ss.str()));
        learningStrategies[strategyIndex]->weight /= totalWeight;
    }
}

LearningModel::~LearningModel() = default;

void LearningModel::writeWeights(unsigned int gameNum) {
    for (size_t strategyIndex = 0; strategyIndex < learningStrategies.size(); strategyIndex++) {
        outputStreams[strategyIndex] << gameNum << " " << learningStrategies[strategyIndex]->weight << std::endl;
    }
}

void LearningModel::pickNewStrategies(double phi, std::vector<Miner *> &miners, const Blockchain &chain) {
    static std::random_device *rd = new std::random_device();
    static std::mt19937 gen((*rd)());
    
    for (size_t minerIndex = 0; minerIndex < minerCount; minerIndex++) {
        std::vector<double> probabilities = probabilitiesForMiner(minerIndex, phi);
        std::discrete_distribution<std::size_t> dis(begin(probabilities), end(probabilities));
        size_t stratIndex = dis(gen);
        chosenStrats[minerIndex] = stratIndex;
        Miner *miner = miners[minerIndex];
        auto &strat = learningStrategies[stratIndex]->strat;
        miner->changeStrategy(*strat, chain);
    }
}

void LearningModel::updateWeights(GameResult &gameResult, Value maxPossibleProfit, double phi) {
    std::vector<Value> profits;
    profits.reserve(minerCount);
    
    for (size_t minerIndex = 0; minerIndex < minerCount; minerIndex++) {
        profits.push_back(gameResult.minerResults[minerIndex].totalProfit);
    }
    
    updateWeights(profits, maxPossibleProfit, phi);
}

void LearningModel::updateWeight(size_t i, StratWeight weight) {
    learningStrategies[i]->weight = weight;
}

std::vector<StratWeight> LearningModel::getCurrentWeights() const {
    std::vector<StratWeight> weights;
    weights.reserve(stratCount);
    std::transform(begin(learningStrategies), end(learningStrategies), std::back_inserter(weights), [](const auto &learningStrategy) { return learningStrategy->weight; });
    return weights;
}

StratWeight LearningModel::getCurrentWeight(size_t i) const {
    return learningStrategies[i]->weight;
}

size_t LearningModel::getChosenStrat(size_t i) const {
    return chosenStrats[i];
}

void LearningModel::printWeights() {
    for (size_t strategyIndex = 0; strategyIndex < learningStrategies.size(); strategyIndex++) {
        std::cout << "strategy:" << learningStrategies[strategyIndex]->strat->name;
        std::cout << " weight: " << learningStrategies[strategyIndex]->weight << "\n";
    }
}

