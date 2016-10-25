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
class LearningMiner;
class Strategy;

class LearningModel {
protected:
    std::map<std::string, std::ofstream> outputStreams;
    std::vector<LearningMiner *> learningMiners;
public:
    LearningModel(std::vector<Strategy> &strategies, std::vector<LearningMiner *> learningMiners, std::string resultFolder);
    virtual ~LearningModel();
    virtual void pickNewStrategies(double phi) = 0;
    virtual void updateWeights(GameResult &gameResult, Value maxPossibleProfit, double phi) = 0;
    virtual void writeWeights(unsigned int gameNum) = 0;
};

#endif /* learning_model_hpp */
