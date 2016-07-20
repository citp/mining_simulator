//
//  multiplicative_weights.cpp
//  BlockSim
//
//  Created by Harry Kalodner on 5/30/16.
//  Copyright © 2016 Harry Kalodner. All rights reserved.
//

#include "multiplicative_weights.hpp"
#include "miner.hpp"
#include "logging.h"
#include "utils.hpp"
#include "minerGroup.hpp"
#include "default_miner.hpp"
#include "miner_result.hpp"

#include <sstream>
#include <cassert>
#include <random>
#include <iostream>

#include <sys/stat.h>
#include <stdlib.h>

MultiplicativeWeights::StratMW::StratMW(Strategy strat_, StratWeight weight_, const std::string &fileName_) :
file(std::ofstream(fileName_)), strat(strat_), originalWeight(weight_), weight(weight_), probability(0), profits(0), minersUsing(0) {}

void MultiplicativeWeights::StratMW::writeWeight(unsigned int gameNum, StratWeight totWeight) {
    file << gameNum << " " << weight / totWeight << std::endl;
}

void MultiplicativeWeights::StratMW::writeWeight(unsigned int gameNum, MinerCount numMiners, MinerCount numDefault) {
    double realWeight = (rawCount(numMiners - numDefault) * rawWeight(weight)) / rawCount(numMiners);
    file << gameNum << " " << realWeight << std::endl;
}


// x / 1

// x * (miners - default) / miners
// ((miners - default)^2 * x) / miners
// when default = 0, ((miners)^2 * x) / miners = miners * x

MultiplicativeWeights::MultiplicativeWeights(std::string resultFolder_, Strategy defaultStrategy_, double alpha_) :
    alpha(alpha_),
    totalWeight(0),
    totalProfits(0),
    maxProfits(0),
    madeProfit(false),
    resultFolder(resultFolder_),
    defaultStrategy(defaultStrategy_)
{
    char final [256];
    sprintf (final, "./%s", resultFolder.c_str());
    mkdir(final,0775);
    calculateProbabilities();
}

MultiplicativeWeights::MultiplicativeWeights(std::string resultFolder_, Strategy defaultStrategy_) : MultiplicativeWeights(resultFolder_, defaultStrategy_, -1) {}

void MultiplicativeWeights::addStrat(const Strategy strategy, StratWeight weight) {
    std::stringstream ss;
    auto size = std::distance(std::begin(customStrats), std::end(customStrats));
    ss << resultFolder << "/index-" << size << "-" << strategy << ".txt";
    customStrats.push_back({strategy, weight, ss.str()});
    totalWeight += weight;
}

const Strategy &MultiplicativeWeights::pickRandomStrategy() const {
    std::vector<double> weights;
    weights.reserve(customStrats.size());
    std::transform(begin(customStrats), end(customStrats), std::back_inserter(weights), [](const StratMW &stratMW) { return stratMW.probability; });
    size_t index = selectWeightedIndex(weights);
    return customStrats[index].strat;
}

void MultiplicativeWeights::updateWeight(StratMW &stratMW, StratWeight weight) {
    totalWeight -= stratMW.weight;
    stratMW.weight = weight;
    totalWeight += stratMW.weight;
}

void MultiplicativeWeights::printWeights() const {
    for (const StratMW &stratMW : customStrats) {
        std::cout << "strategy: " << stratMW.strat;
        std::cout << " weight:" << stratMW.weight << std::endl;
    }
    std::cout << "total weight:" << totalWeight << std::endl;
}

void MultiplicativeWeights::clearProfits() {
    for (StratMW &stratMW : customStrats) {
        stratMW.profits = 0;
        stratMW.minersUsing = 0;
    }
    totalProfits = 0;
    maxProfits = 0;
    madeProfit = false;
}

void MultiplicativeWeights::addProfits(const Miner &miner, MinerResult result) {
    auto stratMWPtr = std::find_if(begin(customStrats), end(customStrats), [&](StratMW &p) { return &p.strat == &miner.strat;});
    assert(stratMWPtr != end(customStrats));
    (*stratMWPtr).profits += result.totalProfit;
    (*stratMWPtr).minersUsing++;;
}

void MultiplicativeWeights::calculateNewNormalizedWeights(const MinerGroup &minerGroup, std::map<const Miner *, MinerResult> minerResults) {
    
    // Clear out profits
    for (StratMW &stratMW : customStrats) {
        stratMW.profits = 0;
        stratMW.minersUsing = 0;
    }
    totalProfits = 0;
    maxProfits = 0;
    madeProfit = false;
    
    // Add new profits from miners
    for (const auto &miner : minerGroup.miners) {
        if (&miner->strat != &defaultStrategy) {
            addProfits(*miner, minerResults[miner.get()]);
        }
    }
    
    // Normalize Profits
    
    for (StratMW &stratMW : customStrats) {
        if (stratMW.minersUsing > MinerCount(1)) {
            stratMW.profits = stratMW.profits / stratMW.minersUsing;
            stratMW.minersUsing = 1; //normalize to 1 miner
        }
        totalProfits += stratMW.profits;
        if (maxProfits < stratMW.profits) {
            maxProfits = stratMW.profits;
            madeProfit = true;
        }
    }
    
    // After time step i, if we used strategy j, update w_ {i+1} j = w_ij * e^{eps r_i / n}, where r_i is our reward during time step i. If we don't use strategy j, keep the weight as is.
    for (StratMW &stratMW : customStrats) {
        if (stratMW.minersUsing != MinerCount(0)) {
            StratWeight newWeight = stratMW.weight * StratWeight(std::exp(getPhi() * rawValue(stratMW.profits/maxProfits)));
            updateWeight(stratMW, newWeight);
        }
    }
    
    calculateProbabilities();
}

double MultiplicativeWeights::getPhi() {
    if (alpha > 0 && alpha < 1) {
        return alpha;
    } else {
        return 1.0 / std::sqrt(customStrats.size() * std::log(customStrats.size()));
    }
}

void MultiplicativeWeights::calculateProbabilities() {
    double phi = getPhi();
    for (StratMW &stratMW : customStrats) {
        stratMW.probability = (1 - phi) * rawWeight(stratMW.weight / totalWeight) + phi * 1.0 / customStrats.size();
    }
}

void MultiplicativeWeights::resetWeights() {
    for (StratMW &stratMW : customStrats) {
        updateWeight(stratMW, stratMW.originalWeight);
    }
    calculateProbabilities();
}

void MultiplicativeWeights::writeWeights(unsigned int gameNum) {
    for (StratMW &stratMW : customStrats) {
        stratMW.writeWeight(gameNum, totalWeight);
    }
}

void MultiplicativeWeights::writeWeights(unsigned int gameNum, MinerCount numMiners, MinerCount numDefault) {
    for (StratMW &stratMW : customStrats) {
        stratMW.writeWeight(gameNum, numMiners, numDefault);
    }
}
