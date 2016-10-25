//
//  learning_model.cpp
//  BlockSim
//
//  Created by Harry Kalodner on 10/24/16.
//  Copyright © 2016 Harry Kalodner. All rights reserved.
//

#include "learning_model.hpp"
#include "strategy.hpp"

#include <sstream>
#include <fstream>

#include <sys/stat.h>

LearningModel::LearningModel(std::vector<Strategy> &strategies, std::vector<LearningMiner *> learningMiners_, std::string resultFolder) : learningMiners(learningMiners_) {
    
    char final [256];
    sprintf (final, "./%s", resultFolder.c_str());
    mkdir(final,0775);
    
    for (auto &strategy : strategies) {
        std::stringstream ss;
        auto size = std::distance(std::begin(outputStreams), std::end(outputStreams));
        ss << resultFolder << "/index-" << size << "-" << strategy << ".txt";
        outputStreams[strategy.name] = std::ofstream(ss.str());
    }
}

LearningModel::~LearningModel() = default;
