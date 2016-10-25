//
//  exp3_learning_model.cpp
//  BlockSim
//
//  Created by Harry Kalodner on 10/24/16.
//  Copyright © 2016 Harry Kalodner. All rights reserved.
//

#include "exp3_learning_model.hpp"

#include "learning_miner.hpp"
#include "game_result.hpp"
#include "miner_result.hpp"

Exp3LearningModel::Exp3LearningModel(std::vector<Strategy> &strategies, std::vector<LearningMiner *> learningMiners, std::string resultFolder) : LearningModel(strategies, learningMiners, resultFolder) {}

void Exp3LearningModel::pickNewStrategies(double phi) {
    for (auto &miner : learningMiners) {
        miner->updateProbabilities(phi);
        miner->pickRandomStrategy();
    }
}

void Exp3LearningModel::updateWeights(GameResult &gameResult, Value maxPossibleProfit, double phi) {
    for (auto &miner : learningMiners) {
        Value profit = gameResult.minerResults[miner].totalProfit;
        miner->updateWeights(profit, maxPossibleProfit, phi);
    }
}

void Exp3LearningModel::writeWeights(unsigned int gameNum) {
    std::map<std::string, double> probabilities;
    
    for (auto &miner : learningMiners) {
        for (auto &learningStrat : miner->customStrats) {
            if (probabilities.find(learningStrat.strat.name) == probabilities.end()) {
                probabilities[learningStrat.strat.name] = 0;
            }
            probabilities[learningStrat.strat.name] += rawWeight(learningStrat.weight);
        }
    }
    
    for(auto const &ent1 : probabilities) {
        outputStreams[ent1.first] << gameNum << " " << ent1.second / learningMiners.size() << std::endl;
    }
}
