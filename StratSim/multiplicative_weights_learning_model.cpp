//
//  multiplicative_weights_learning_model.cpp
//  BlockSim
//
//  Created by Harry Kalodner on 10/24/16.
//  Copyright © 2016 Harry Kalodner. All rights reserved.
//

#include "multiplicative_weights_learning_model.hpp"

#include "BlockSim/utils.hpp"
#include "learning_strategy.hpp"

#include <assert.h>
#include <math.h>
#include <algorithm>

MultiplicativeWeightsLearningModel::MultiplicativeWeightsLearningModel(std::vector<std::unique_ptr<LearningStrategy>> &learningStrategies, size_t minerCount_, std::string resultFolder) : LearningModel(learningStrategies, minerCount_, resultFolder) {
    std::vector<StratWeight> weights = getCurrentWeights();
    probabilities.reserve(stratCount);
    std::transform(begin(weights), end(weights), std::back_inserter(probabilities), [](const auto &weight) { return rawWeight(weight); });
}

std::vector<double> MultiplicativeWeightsLearningModel::probabilitiesForMiner(size_t, double) {
    return probabilities;
}

void MultiplicativeWeightsLearningModel::updateWeights(std::vector<Value> profits, Value, double phi) {

    struct StratUsage {
        Value profits;
        MinerCount minersUsing;
        
        StratUsage() : profits(0), minersUsing(0) {}
        
        void addProfit(Value profit) {
            profits += profit;
            minersUsing++;
        }
        
        void normalize() {
            if (minersUsing > MinerCount(1)) {
                profits /= Value(minersUsing);
                minersUsing = MinerCount(1);
            }
        }
        
        bool isUnused() {
            return minersUsing == MinerCount(0);
        }
        
        double profitRatio(Value maxProfits) {
            assert(profits <= maxProfits);
            return rawValue(profits/maxProfits);
        }
    };
    
    std::vector<StratUsage> stratUsages;
    stratUsages.resize(stratCount);
    
    for (size_t i = 0; i < minerCount; i++) {
        stratUsages[getChosenStrat(i)].addProfit(profits[i]);
    }
    
    Value maxProfits(0);
    for (auto &stratUsage : stratUsages) {
        stratUsage.normalize();
        if (maxProfits < stratUsage.profits) {
            maxProfits = stratUsage.profits;
        }
    }
    
    std::vector<StratWeight> newWeights;
    newWeights.reserve(stratCount);
    
    StratWeight totalWeight(0);
    for (size_t i = 0; i < stratUsages.size(); i++) {
        double c_t = 1.1 - stratUsages[i].profitRatio(maxProfits);
        if (stratUsages[i].isUnused()) {
            //means no one tried the strategy, keep weight unchanged (encourage exploration)?
            c_t = 0;
        }
        
        StratWeight newWeight = getCurrentWeight(i) * StratWeight(pow((1-phi), c_t));
        updateWeight(i, newWeight);
        totalWeight += newWeight;
        newWeights.push_back(newWeight);
    }
    
    for (size_t i = 0; i < newWeights.size(); i++) {
        updateWeight(i, newWeights[i] / totalWeight);
    }
    
    std::vector<StratWeight> weights = getCurrentWeights();
    
    probabilities.clear();
    probabilities.reserve(stratCount);
    std::transform(begin(weights), end(weights), std::back_inserter(probabilities), [](const auto &weight) { return rawWeight(weight); });
}
