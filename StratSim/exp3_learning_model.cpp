//
//  exp3_learning_model.cpp
//  BlockSim
//
//  Created by Harry Kalodner on 10/24/16.
//  Copyright © 2016 Harry Kalodner. All rights reserved.
//

#include "exp3_learning_model.hpp"

#include "learning_strategy.hpp"

#include "BlockSim/utils.hpp"

#include <math.h>
#include <assert.h>

Exp3LearningModel::Exp3LearningModel(std::vector<std::unique_ptr<LearningStrategy>> &learningStrategies_, size_t minerCount_, std::string resultFolder) : LearningModel(learningStrategies_, minerCount_, resultFolder) {
    std::vector<StratWeight> weights = getCurrentWeights();
    minersWeights.reserve(minerCount);
    minersProbabilities.resize(minerCount);
    std::vector<double> probabilities;
    for (size_t i = 0; i < minerCount_; i++) {
        minersWeights.push_back(weights);
    }
}

std::vector<double> Exp3LearningModel::probabilitiesForMiner(size_t minerIndex, double phi) {
    
    std::vector<double> probabilities;
    probabilities.reserve(stratCount);
    for (size_t strategyIndex = 0; strategyIndex < stratCount; strategyIndex++) {
        double probability = (1 - phi) * rawWeight(minersWeights[minerIndex][strategyIndex]) + phi / stratCount;
        
        assert(probability > 0);
        assert(!isnan(probability) && !isinf(probability));
        assert(isfinite(probability) && isnormal(probability));
        
        probabilities.push_back(probability);
    }
    minersProbabilities[minerIndex] = probabilities;
    
    return minersProbabilities[minerIndex];
}

void Exp3LearningModel::updateWeights(std::vector<Value> profits, Value maxPossibleProfit, double phi) {
    
    for (size_t minerIndex = 0; minerIndex < minerCount; minerIndex++) {
        double profitRatio = valuePercentage(profits[minerIndex], maxPossibleProfit);
        double normalizedProfit(fmin(profitRatio, 1.0));
        
        // Step 4 and Step 5
        double gHat = normalizedProfit / minersProbabilities[minerIndex][getChosenStrat(minerIndex)];
        double weightAdjustment = exp((phi * gHat) / stratCount);
        
        StratWeight oldWeight = minersWeights[minerIndex][getChosenStrat(minerIndex)];
        minersWeights[minerIndex][getChosenStrat(minerIndex)] *= StratWeight(weightAdjustment);
        StratWeight maxWeight = StratWeight(1) - oldWeight + minersWeights[minerIndex][getChosenStrat(minerIndex)];
        
        // Normalize weights
        
        for (size_t strategyIndex = 0; strategyIndex < stratCount; strategyIndex++) {
            minersWeights[minerIndex][strategyIndex] /= maxWeight;
        }
    }
    
    std::vector<StratWeight> weights;
    weights.resize(stratCount);
    
    for (auto &minerWeights : minersWeights) {
        for (size_t i = 0; i < stratCount; i++) {
            weights[i] += minerWeights[i];
        }
    }
    
    for (size_t i = 0; i < stratCount; i++) {
        updateWeight(i, weights[i] / StratWeight(minerCount));
    }
}
