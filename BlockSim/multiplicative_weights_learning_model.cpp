//
//  multiplicative_weights_learning_model.cpp
//  BlockSim
//
//  Created by Harry Kalodner on 10/24/16.
//  Copyright © 2016 Harry Kalodner. All rights reserved.
//

#include "multiplicative_weights_learning_model.hpp"

#include "learning_miner.hpp"
#include "game_result.hpp"
#include "miner_result.hpp"
#include "utils.hpp"

#include <assert.h>
#include <math.h>

MultiplicativeWeightsLearningModel::StratMW::StratMW(Strategy strat_, StratWeight weight_) : strat(strat_), weight(weight_), profits(0), minersUsing(0) {}

MultiplicativeWeightsLearningModel::MultiplicativeWeightsLearningModel(std::vector<Strategy> &strategies, std::vector<LearningMiner *> learningMiners, std::string resultFolder) : LearningModel(strategies, learningMiners, resultFolder) {
    
    StratWeight weight(1.0 / strategies.size());
    
    for (auto &strat : strategies) {
        customStrats.push_back({strat, weight});
    }
}

void MultiplicativeWeightsLearningModel::pickNewStrategies(double) {
    for (auto &miner : learningMiners) {
        std::vector<double> weights;
        weights.reserve(customStrats.size());
        std::transform(begin(customStrats), end(customStrats), std::back_inserter(weights), [](const StratMW &stratMW) { return rawWeight(stratMW.weight); });
        size_t index = selectWeightedIndex(weights);
        miner->pickStrategy(index);
    }
}

void MultiplicativeWeightsLearningModel::updateWeights(GameResult &gameResult, Value, double phi) {
    for (StratMW &stratMW : customStrats) {
        stratMW.profits = 0;
        stratMW.minersUsing = 0;
    }
    
    for (auto &miner : learningMiners) {
        customStrats[miner->stratIndex].profits += gameResult.minerResults[miner].totalProfit;
        customStrats[miner->stratIndex].minersUsing++;
    }
    
    Value maxProfits(0);
    
    for (StratMW &stratMW : customStrats) {
        if (stratMW.minersUsing > MinerCount(1)) {
            stratMW.profits = stratMW.profits / stratMW.minersUsing;
            stratMW.minersUsing = 1; //normalize to 1 miner
        }
        if (maxProfits < stratMW.profits) {
            maxProfits = stratMW.profits;
        }
    }
    
    StratWeight totalWeight(0);
    
    for (StratMW &stratMW : customStrats) {
        double c_t = 1.1 - rawValue(stratMW.profits/maxProfits);
        if (stratMW.minersUsing == MinerCount(0)) {
            //means no one tried the strategy, keep weight unchanged (encourage exploration)?
            c_t = 0;
        }
        stratMW.weight *= StratWeight(pow((1-phi), c_t));
        totalWeight += stratMW.weight;
    }
    
    for (StratMW &stratMW : customStrats) {
        stratMW.weight /= totalWeight;
    }
}

void MultiplicativeWeightsLearningModel::writeWeights(unsigned int gameNum) {
    for (StratMW &stratMW : customStrats) {
        outputStreams[stratMW.strat.name] << gameNum << " " << stratMW.weight << std::endl;
    }
}

