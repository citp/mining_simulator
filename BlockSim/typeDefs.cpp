//
//  typeDefs.cpp
//  BlockSim
//
//  Created by Harry Kalodner on 6/15/16.
//  Copyright © 2016 Harry Kalodner. All rights reserved.
//

#include "typeDefs.hpp"

#include "arithmetic_type/primitive_cast.hpp"

#ifdef DEBUG_TYPES

TimeType rawTime(BlockTime time) {
    return arithmetic::primitive_cast(time);
}

HeightType rawHeight(BlockHeight height) {
    return arithmetic::primitive_cast(height);
}

ValueType rawValue(Value value) {
    return arithmetic::primitive_cast(value);
}

ValueType rawValue(BlockValue value) {
    return arithmetic::primitive_cast(value);
}

ValueType rawValue(ValueRate value) {
    return arithmetic::primitive_cast(value);
}

CountType rawCount(MinerCount count) {
    return arithmetic::primitive_cast(count);
}

CountType rawCount(BlockCount count) {
    return arithmetic::primitive_cast(count);
}

TimeType rawBlockRate(BlockRate rate) {
    return arithmetic::primitive_cast(rate);
}

RateType rawRate(TimeRate rate) {
    return arithmetic::primitive_cast(rate);
}

RateType rawRate(HashRate rate) {
    return arithmetic::primitive_cast(rate);
}

WeightType rawWeight(StratWeight weight) {
    return arithmetic::primitive_cast(weight);
}

BlockTime operator% (BlockTime x, const BlockTime& y) {
    return BlockTime(rawTime(x) % rawTime(y));
}

// (money / second) * seconds = money
Value operator* (ValueRate x, const BlockTime& y) {
    return Value(rawTime(y) * rawValue(x));
}

// (1 / block) / (seconds / block) = (1 / second)
TimeRate operator/ (const HashRate& y, BlockRate x) {
    return TimeRate(rawRate(y) / rawBlockRate(x));
}

// (1 / block) * money = (money / block)
ValueRate operator* (TimeRate x, const Value& y) {
    return ValueRate(static_cast<ValueType>(rawValue(y) * rawRate(x)));
}

// (money / second) * (seconds / block) = (money / block)
BlockValue operator* (const ValueRate& y, BlockRate x) {
    return BlockValue(rawValue(y) * rawBlockRate(x));
}

// (money / block) / (seconds / block) = (money / second)
ValueRate operator/ (const BlockValue& y, BlockRate x) {
    return ValueRate(rawValue(y) / rawBlockRate(x));
}

// blocks * (seconds / block) = seconds
BlockTime operator* (const BlockCount& y, BlockRate x) {
    return BlockTime(rawCount(y) * rawBlockRate(x));
}

double operator/ (const Value& y, StratWeight x) {
    return rawValue(y) / rawWeight(x);
}

Value operator/ (const Value& y, MinerCount x) {
    return Value(rawValue(y) / rawCount(x));
}

#else

TimeType rawTime(BlockTime time) {
    return time;
}
HeightType rawHeight(BlockHeight height) {
    return height;
}

ValueType rawValue(Value value) {
    return value;
}

CountType rawCount(MinerCount count) {
    return count;
}

TimeType rawBlockRate(BlockRate rate) {
    return rate;
}

RateType rawRate(TimeRate rate) {
    return rate;
}

WeightType rawWeight(StratWeight weight) {
    return weight;
}

#endif

double valuePercentage(Value a, Value b) {
    return static_cast<double>(rawValue(a)) / static_cast<double>(rawValue(b));
}
