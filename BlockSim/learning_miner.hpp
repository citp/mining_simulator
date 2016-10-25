//
//  learning_miner.hpp
//  BlockSim
//
//  Created by Harry Kalodner on 10/17/16.
//  Copyright © 2016 Harry Kalodner. All rights reserved.
//

#ifndef learning_miner_hpp
#define learning_miner_hpp

#include "miner.hpp"

class LearningMiner : public Miner {

private:
    void print(std::ostream& os) const;
public:
    LearningMiner(MinerParameters params_, std::vector<Strategy> &strategies);
    
    struct LearningStrat {
        const Strategy strat;
        
        StratWeight weight;  //used for multiplicative weights to decide how to play
        double probability;
        
        LearningStrat(Strategy strat, StratWeight weight);
    };
    
    std::vector<LearningStrat> customStrats;
    size_t stratIndex;
    
    const Strategy& currentStrategy() const;
    void updateProbabilities(double phi);
    void pickRandomStrategy();
    void pickStrategy(size_t index);
    void updateWeights(Value profit, Value maxPossibleProfit, double phi);
    void writeWeights(unsigned int gameNum);
};

#endif /* learning_miner_hpp */
