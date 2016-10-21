//
//  minerGroup.hpp
//  BlockSim
//
//  Created by Harry Kalodner on 6/9/16.
//  Copyright © 2016 Harry Kalodner. All rights reserved.
//

#ifndef minerGroup_hpp
#define minerGroup_hpp

#include "typeDefs.hpp"
#include "publisherQueue.hpp"
#include "strategy.hpp"

#include <vector>
#include <fstream>
#include <map>

class Miner;
class LearningMiner;
class Blockchain;
class MultiplicativeWeights;
struct GameResult;
class Strategy;


class MinerGroup {
private:
    std::map<std::string, std::ofstream> outputStreams;
public:
    MinerGroup(std::vector<LearningMiner *> learningMiners, std::vector<std::unique_ptr<Miner>> miners, std::vector<Strategy> &strategies, std::string resultFolder);
    
    void initialize(const Blockchain &blockchain);
    
    std::vector<std::unique_ptr<Miner>> miners;
    std::vector<LearningMiner *> learningMiners;
    PublisherQueue publisherQueue;
    
    
    friend std::ostream& operator<<(std::ostream& os, const MinerGroup& minerGroup);
    
    BlockTime nextEventTime();
    void nextMineRound(const Blockchain &blockchain);
    void nextPublishRound(Blockchain &blockchain);
    
    void updateProbabilities(double phi);
    void resetAndpickNewStrategies();
    void updateWeights(GameResult &gameResult, Value maxPossibleProfit, double phi);
    void writeWeights(unsigned int gameNum);
    void resetOrder();
};

#endif /* MinerGroup_hpp */
