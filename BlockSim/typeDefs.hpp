//
//  typeDefs.hpp
//  BlockSim
//
//  Created by Harry Kalodner on 6/15/16.
//  Copyright © 2016 Harry Kalodner. All rights reserved.
//

#ifndef typeDefs_hpp
#define typeDefs_hpp

typedef unsigned long long ValueType;
typedef unsigned long TimeType;
typedef unsigned int HeightType;
typedef unsigned int CountType;
typedef double RateType;
typedef double WeightType;

//#define DEBUG_TYPES

#ifdef DEBUG_TYPES

#include "arithmetic_type/arithmetic_type.hpp"
#include "arithmetic_type/arithmetic_ostream.hpp"

struct Tag1 {};
struct Tag2 {};
struct Tag3 {};
struct Tag4 {};
struct Tag5 {};
struct Tag6 {};
struct Tag7 {};
struct Tag8 {};
struct Tag9 {};
struct Tag10 {};
struct Tag11 {};

using HashRate = arithmetic::Arithmetic<RateType, Tag1>; // 1 / Block
using TimeRate = arithmetic::Arithmetic<RateType, Tag2>; // 1 / second

using Value = arithmetic::Arithmetic<ValueType, Tag3>; // block
using ValueRate = arithmetic::Arithmetic<ValueType, Tag4>; // money / second
using BlockValue = arithmetic::Arithmetic<ValueType, Tag5>; // money / block

using BlockHeight = arithmetic::Arithmetic<HeightType, Tag6>; // block
using BlockTime = arithmetic::Arithmetic<TimeType, Tag7>; // seconds
using BlockRate = arithmetic::Arithmetic<TimeType, Tag8>; // seconds / block
using BlockCount = arithmetic::Arithmetic<CountType, Tag9>; // block

using StratWeight = arithmetic::Arithmetic<WeightType, Tag10>; // block
using MinerCount = arithmetic::Arithmetic<CountType, Tag11>; // block

BlockTime operator% (BlockTime x, const BlockTime& y);

// (money / second) * seconds = money
Value operator* (ValueRate x, const BlockTime& y);

// (1 / block) / (seconds / block) = (1 / second)
TimeRate operator/ (const HashRate& y, BlockRate x);

// (1 / block) * money = (money / block)
ValueRate operator* (TimeRate x, const Value& y);

// (money / second) * (seconds / block) = (money / block)
BlockValue operator* (const ValueRate& y, BlockRate x);

// (money / block) / (seconds / block) = (money / second)
ValueRate operator/ (const BlockValue& y, BlockRate x);

// blocks * (seconds / block) = seconds
BlockTime operator* (const BlockCount& y, BlockRate x);

double operator/ (const Value& y, StratWeight x);

Value operator/ (const Value& y, MinerCount x);


#else

using HashRate = RateType;
using TimeRate = RateType;

using Value = ValueType;
using ValueRate = ValueType;
using BlockValue = ValueType;

using BlockHeight = HeightType;
using BlockTime = TimeType;
using BlockRate = TimeType;
using BlockCount = CountType;

using StratWeight = WeightType;
using MinerCount = CountType;

#endif

TimeType rawTime(BlockTime time);
HeightType rawHeight(BlockHeight height);

ValueType rawValue(Value value);
ValueType rawValue(BlockValue value);
ValueType rawValue(ValueRate value);

CountType rawCount(MinerCount count);
CountType rawCount(BlockCount count);

TimeType rawBlockRate(BlockRate rate);

RateType rawRate(TimeRate rate);
RateType rawRate(HashRate rate);

WeightType rawWeight(StratWeight weight);

double valuePercentage(Value a, Value b);

//#define NDEBUG

#define SATOSHI_PER_BITCOIN ValueType(100000000)
//#define TOTAL_BLOCK_VALUE 50 * SATOSHI_PER_BITCOIN
#define UNDERCUT_VALUE Value(100000)

//#define UNDERCUT_VALUE Value(.001)


#define COST_PER_SEC_TO_MINE ValueRate(0)//(((A*SEC_PER_BLOCK+B)/SEC_PER_BLOCK)*(1.0/NUM_MINERS))*(1.0/2.0)
//assumes block becomes profitable half way and miners have equal hash power


#endif /* typeDefs_hpp */
