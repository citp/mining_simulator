//
//  learning_model.hpp
//  BlockSim
//
//  Created by Harry Kalodner on 10/24/16.
//  Copyright © 2016 Harry Kalodner. All rights reserved.
//

#ifndef learning_model_hpp
#define learning_model_hpp

#include "typeDefs.hpp"

#include <stdio.h>
#include <vector>
#include <map>

struct GameResult;
class LearningStrategy;
class Strategy;
class Miner;
class Blockchain;

class LearningModel {
private:
    std::vector<std::ofstream> outputStreams;
    std::vector<std::unique_ptr<LearningStrategy>> learningStrategies;
    std::vector<size_t> chosenStrats;
protected:
    const size_t stratCount;
    const size_t minerCount;
    
    void updateWeight(size_t i, StratWeight weight);
    
    std::vector<StratWeight> getCurrentWeights() const;
    StratWeight getCurrentWeight(size_t i) const;
    size_t getChosenStrat(size_t i) const;
public:
    
    LearningModel(std::vector<std::unique_ptr<LearningStrategy>> &learningStrategies, size_t minerCount, std::string resultFolder);
    virtual ~LearningModel();
    
    virtual void updateWeights(std::vector<Value> profits, Value maxPossibleProfit, double phi) = 0;
    virtual std::vector<double> probabilitiesForMiner(size_t minerIndex, double phi) = 0;
    
    void pickNewStrategies(double phi, std::vector<Miner *> &miners, const Blockchain &chain);
    void updateWeights(GameResult &gameResult, Value maxPossibleProfit, double phi);
    void writeWeights(unsigned int gameNum);
    void printWeights();
};

#endif /* learning_model_hpp */
