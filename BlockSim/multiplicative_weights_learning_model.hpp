//
//  multiplicative_weights_learning_model.hpp
//  BlockSim
//
//  Created by Harry Kalodner on 10/24/16.
//  Copyright © 2016 Harry Kalodner. All rights reserved.
//

#ifndef multiplicative_weights_learning_model_hpp
#define multiplicative_weights_learning_model_hpp

#include "learning_model.hpp"
#include "strategy.hpp"

#include <stdio.h>

class MultiplicativeWeightsLearningModel : public LearningModel {
private:
    struct StratMW {
        const Strategy strat;
        
        StratWeight weight;  //used for multiplicative weights to decide how to play
        Value profits; //used to calculate the value of this strat at the end
        MinerCount minersUsing;
        
        StratMW(Strategy strat, StratWeight weight);
    };
    
    std::vector<StratMW> customStrats;

public:
    MultiplicativeWeightsLearningModel(std::vector<Strategy> &strategies, std::vector<LearningMiner *> learningMiners, std::string resultFolder);
    void pickNewStrategies(double phi) override;
    void updateWeights(GameResult &gameResult, Value maxPossibleProfit, double phi) override;
    void writeWeights(unsigned int gameNum) override;
};

#endif /* multiplicative_weights_learning_model_hpp */
