//
//  multiplicative_weights.hpp
//  BlockSim
//
//  Created by Harry Kalodner on 5/25/16.
//  Copyright © 2016 Harry Kalodner. All rights reserved.
//

#ifndef multiplicative_weights_hpp
#define multiplicative_weights_hpp

#include "typeDefs.hpp"
#include "strategy.hpp"

#include <fstream>
#include <vector>
#include <map>

class Miner;
struct MinerResult;
class MinerGroup;

class MultiplicativeWeights {
    
private:
    
    struct StratMW {
        std::ofstream file;
        const Strategy strat;
        const StratWeight originalWeight;
        
        StratWeight weight;  //used for multiplicative weights to decide how to play
        double probability;
        Value profits; //used to calculate the value of this strat at the end
        MinerCount minersUsing;
        
        StratMW(Strategy strat, StratWeight weight, const std::string &fileName);
        
        void writeWeight(unsigned int gameNum, StratWeight totalWeight);
        void writeWeight(unsigned int gameNum, MinerCount numMiners, MinerCount numDefault);
    };
    double alpha;
    std::vector<StratMW> customStrats;
    
    StratWeight totalWeight;
    Value totalProfits;
    Value maxProfits;
    bool madeProfit;
    
    void updateWeight(StratMW &stratMW, StratWeight weight);
    void addProfits(const Miner &miner, MinerResult result);
    void calculateProbabilities();
    double getPhi();
public:
    const std::string resultFolder;
    const Strategy defaultStrategy;
    
    
    MultiplicativeWeights(std::string resultFolder, Strategy defaultStrategy, double alpha);
    MultiplicativeWeights(std::string resultFolder, Strategy defaultStrategy);
    
    void addStrat(const Strategy strategy, StratWeight weight);
    
    void clearProfits();
    void writeWeights(unsigned int gameNum);
    void writeWeights(unsigned int gameNum, MinerCount numMiners, MinerCount numDefault);
    
    void calculateNewNormalizedWeights(const MinerGroup &minerGroup, std::map<const Miner *, MinerResult> minerResults);
    void resetWeights();
    
    const Strategy &pickRandomStrategy() const;
    
    void printWeights() const;
};

#endif /* multiplicative_weights_hpp */
