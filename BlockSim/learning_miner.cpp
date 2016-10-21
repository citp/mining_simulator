//
//  learning_miner.cpp
//  BlockSim
//
//  Created by Harry Kalodner on 10/17/16.
//  Copyright © 2016 Harry Kalodner. All rights reserved.
//

#include "learning_miner.hpp"
#include "simple_mining_style.hpp"
#include "picky_mining_style.hpp"
#include "simple_publisher.hpp"
#include "utils.hpp"
#include "minerImp.hpp"
#include "block.hpp"

#include <cmath>
#include <assert.h>

LearningMiner::LearningStrat::LearningStrat(Strategy strat_, StratWeight weight_) : strat(strat_), weight(weight_), probability(0) {}

LearningMiner::LearningMiner(MinerParameters params_, std::vector<Strategy> &strategies) : Miner(params_, strategies.front().createImp()), stratIndex(0) {
//    double dominantStrat = .0001;
    
//    unsigned long i = 0;
    for (auto &strategy : strategies) {
        StratWeight weight(1.0 / strategies.size());
        customStrats.push_back({strategy, weight});
//        if (i < (strategies.size() - 1)) {
//            customStrats.push_back({strategy, StratWeight((1.0 - dominantStrat) / (strategies.size() - 1))});
//        } else {
//            customStrats.push_back({strategy, StratWeight(dominantStrat)});
//        }
//        i++;
    }
}

void LearningMiner::updateProbabilities(double phi) {
    
    // Step 1
    
    double phiFactor = phi / customStrats.size();
    for (LearningStrat &learningStrat : customStrats) {
        learningStrat.probability = (1 - phi) * rawWeight(learningStrat.weight) + phiFactor;
        assert(learningStrat.probability > 0);
        assert(!std::isnan(learningStrat.probability) && !std::isinf(learningStrat.probability));
        assert(std::isfinite(learningStrat.probability) && std::isnormal(learningStrat.probability));
    }
}

void LearningMiner::pickRandomStrategy() {
    
    // Step 2
    
    std::vector<double> probabilities;
    probabilities.reserve(customStrats.size());
    std::transform(begin(customStrats), end(customStrats), std::back_inserter(probabilities), [](const LearningStrat &learningStrat) { return learningStrat.probability; });
    size_t index = selectWeightedIndex(probabilities);
    changeImplementation(customStrats[index].strat.createImp());
    stratIndex = index;
}

void LearningMiner::updateWeights(Value profit, Value maxPossibleProfit, double phi) {
    // Should compare same rather than name
    
    // Step 3
    // Normalize profit betwen 0 and 1
    Value normalizedProfit(0);
    
    if(profit > maxPossibleProfit) {
        normalizedProfit = 1; // Shouldn't happen often
    } else {
        normalizedProfit = profit / maxPossibleProfit;
    }
    
    // Step 4 and Step 5
    double gHat = rawValue(normalizedProfit) / customStrats[stratIndex].probability;
//    assert(std::isfinite(gHat) && std::isnormal(gHat));
    double weightAdjustment = std::exp((phi * gHat) / customStrats.size());
//    assert(std::isfinite(weightAdjustment) && std::isnormal(weightAdjustment));
//    assert(weightAdjustment >= 1);
    StratWeight newWeight = customStrats[stratIndex].weight * StratWeight(weightAdjustment);
    
    double weightDebug = rawWeight(newWeight);
//    assert(!std::isnan(weightDebug) && weightDebug > 0 && !std::isinf(weightDebug));
//    assert(std::isfinite(weightDebug) && std::isnormal(weightDebug));
    
    StratWeight maxWeight = StratWeight(1) + newWeight - customStrats[stratIndex].weight;
    
    customStrats[stratIndex].weight = newWeight;
    
    // Normalize weights
    for (LearningStrat &learningStrat : customStrats) {
        learningStrat.weight /= maxWeight;
    }
}

void LearningMiner::print(std::ostream& os) const {
    os << "[" << customStrats[stratIndex].strat.name << "] ";
    Miner::print(os);
}

