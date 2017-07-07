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

#include <stdio.h>

class MultiplicativeWeightsLearningModel : public LearningModel {
private:
    std::vector<double> probabilities;
    
    std::vector<double> probabilitiesForMiner(size_t minerIndex, double phi) override;
    void updateWeights(std::vector<Value> profits, Value maxPossibleProfit, double phi) override;
public:
    MultiplicativeWeightsLearningModel(std::vector<std::unique_ptr<LearningStrategy>> &learningStrategies, size_t minerCount, std::string resultFolder);
    
};

#endif /* multiplicative_weights_learning_model_hpp */
