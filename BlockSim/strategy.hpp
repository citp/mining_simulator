//
//  strategy.hpp
//  BlockSim
//
//  Created by Harry Kalodner on 5/31/16.
//  Copyright © 2016 Harry Kalodner. All rights reserved.
//

#ifndef strategy_hpp
#define strategy_hpp

#include "typeDefs.hpp"

#include <functional>
#include <memory>
#include <string>

class Block;
class MinedBlock;
class Blockchain;
class MiningStyle;
class PublishingStrategy;
class Miner;
class MinerImp;

typedef std::function<std::unique_ptr<MinerImp>()> MinerImpCreator;


class Strategy {
private:
    const MinerImpCreator minerImpCreator;

public:
    const std::string name;
    
    Strategy(std::string name, MinerImpCreator minerImpCreator);
    friend std::ostream& operator<<(std::ostream& os, const Strategy& strat);
    
    std::unique_ptr<MinerImp> createImp() const;
};

#endif /* strategy_hpp */
