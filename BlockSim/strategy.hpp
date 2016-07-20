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

class Miner;
struct MinerParameters;
class Strategy;

typedef std::function<std::unique_ptr<Miner>(MinerParameters, const Strategy &)>  MinerCreatePtr;

class Strategy {
    
public:
    const std::string name;
    const MinerCreatePtr creator;
    
    Strategy(std::string name, MinerCreatePtr creator);
    
    std::unique_ptr<Miner> generateMiner(MinerParameters params) const;

    friend std::ostream& operator<<(std::ostream& os, const Strategy& strat);
};

#endif /* strategy_hpp */
