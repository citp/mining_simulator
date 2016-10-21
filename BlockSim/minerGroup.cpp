//
//  minerGroup.cpp
//  BlockSim
//
//  Created by Harry Kalodner on 6/9/16.
//  Copyright © 2016 Harry Kalodner. All rights reserved.
//

#include "minerGroup.hpp"

#include "miner.hpp"
#include "learning_miner.hpp"
#include "default_miner.hpp"
#include "minerParameters.h"
#include "publishing_strategy.hpp"
#include "blockchain.hpp"
#include "block.hpp"
#include "mining_style.hpp"
#include "game_result.hpp"
#include "miner_result.hpp"
#include "minerImp.hpp"

#include <cassert>
#include <sstream>


bool miningSort(const std::unique_ptr<Miner>& miner1, const std::unique_ptr<Miner>& miner2);
bool miningSort(const std::unique_ptr<Miner>& miner1, const std::unique_ptr<Miner>& miner2)  {
    auto miner1Time = miner1->nextMiningTime();
    auto miner2Time = miner2->nextMiningTime();
    if (miner1Time < miner2Time) {
        return false;
    } else if (miner1Time > miner2Time) {
        return true;
    } else {
        return miner1->params.number < miner2->params.number;
    }
}

MinerGroup::MinerGroup(std::vector<LearningMiner *> learningMiners_, std::vector<std::unique_ptr<Miner>> miners_, std::vector<Strategy> &strategies, std::string resultFolder) : miners(std::move(miners_)), learningMiners(learningMiners_), publisherQueue(miners) {
    for (auto &strategy : strategies) {
        std::stringstream ss;
        auto size = std::distance(std::begin(outputStreams), std::end(outputStreams));
        ss << resultFolder << "/index-" << size << "-" << strategy << ".txt";
        outputStreams[strategy.name] = std::ofstream(ss.str());
    }
}

void MinerGroup::initialize(const Blockchain &blockchain) {
    for (auto &miner : miners) {
        miner->initialize(blockchain);
    }
    std::make_heap(begin(miners), end(miners), miningSort);
}

BlockTime MinerGroup::nextEventTime() {
    auto nextMining = miners.front()->nextMiningTime();
    auto nextPublish = publisherQueue.nextPublishingTime();
    return std::min(nextMining, nextPublish);}

void MinerGroup::nextMineRound(const Blockchain &blockchain) {
    assert(miners.front()->nextMiningTime() >= blockchain.getTime());
    while (miners.front()->nextMiningTime() == blockchain.getTime()) {
        std::pop_heap(begin(miners), end(miners), miningSort);
        
        // Must update order of publishing queue
        publisherQueue.removePublisher(miners.back().get());
        // Mine for blocks
        miners.back()->miningPhase(blockchain);
        
        publisherQueue.pushPublisher(miners.back().get());
        
        std::push_heap(begin(miners), end(miners), miningSort);
    }
}

void MinerGroup::nextPublishRound(Blockchain &blockchain) {
    assert(publisherQueue.nextPublishingTime() >= blockchain.getTime());
    while (publisherQueue.nextPublishingTime() == blockchain.getTime()) {
        auto firstPublisher = publisherQueue.popNextPublisher();
        // Publish blocks
        firstPublisher->publishPhase(blockchain);
        publisherQueue.pushPublisher(firstPublisher);
    }
}


std::ostream& operator<<(std::ostream& os, const MinerGroup& minerGroup) {
    for (const auto &miner : minerGroup.miners) {
        os << *miner << std::endl;
    }
    return os;
}

void MinerGroup::updateProbabilities(double phi) {
    for (auto &miner : learningMiners) {
        miner->updateProbabilities(phi);
    }
}

void MinerGroup::resetAndpickNewStrategies() {
    MinerCount i(0);
    
    for (auto &miner : learningMiners) {
        miner->pickRandomStrategy();
    }
}

void MinerGroup::resetOrder() {
    std::make_heap(begin(miners), end(miners), miningSort);
    publisherQueue = PublisherQueue(miners);
}

void MinerGroup::updateWeights(GameResult &gameResult, Value maxPossibleProfit, double phi) {
    for (auto &miner : learningMiners) {
        Value profit = gameResult.minerResults[miner].totalProfit;
        miner->updateWeights(profit, maxPossibleProfit, phi);
    }
}

void MinerGroup::writeWeights(unsigned int gameNum) {
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
