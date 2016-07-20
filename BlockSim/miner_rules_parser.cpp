//
//  miner_rules_parser.cpp
//  BlockSim
//
//  Created by Harry Kalodner on 6/14/16.
//  Copyright © 2016 Harry Kalodner. All rights reserved.
//

#include "miner_rules_parser.hpp"

//#include <boost/spirit.hpp>
//
//void test();
//
//void test() {
//    using namespace boost::spirit;
//    auto parentKeyword = str_p("Which Block");
//    auto valueHead = str_p("How much");
//    auto publisherHead = str_p("Publish?");
//    auto existsKeyword = str_p("exists");
//    
//    auto blockToMine = str_p("B^*(m)");
//    auto ownBlock = str_p("Own-B^m_(h^*)");
//    
//    
//    rule<> boolExpr;
//    rule<> codeBlock;
//    
//    auto ifStatement = str_p("if") >> confix_p('(', boolExpr, ')');
//    auto elseIfStatement = str_p("elseif") >> confix_p('(', boolExpr, ')');
//    auto elseStatement = str_p("else");
//    
//    auto ifClause = ifStatement >> codeBlock >> *(elseIfStatement >> codeBlock) >> !(elseStatement >> codeBlock);
//    
//    
////    auto assignment =
//}
