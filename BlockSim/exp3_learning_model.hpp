//
//  exp3_learning_model.hpp
//  BlockSim
//
//  Created by Harry Kalodner on 10/24/16.
//  Copyright © 2016 Harry Kalodner. All rights reserved.
//

#ifndef exp3_learning_model_hpp
#define exp3_learning_model_hpp

#include "learning_model.hpp"

#include <stdio.h>

class Exp3LearningModel : public LearningModel {
    void pickNewStrategies(double phi) override;
    void updateWeights(GameResult &gameResult, Value maxPossibleProfit, double phi) override;
    void writeWeights(unsigned int gameNum) override;
    
public:
    Exp3LearningModel(std::vector<Strategy> &strategies, std::vector<LearningMiner *> learningMiners, std::string resultFolder);
};



#endif /* exp3_learning_model_hpp */
