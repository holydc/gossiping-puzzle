#include <fstream>
#include <iterator>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

std::vector<std::string> split(const std::string &s) {
  std::istringstream iss(s);
  std::istream_iterator<std::string> begin(iss), end;
  return std::vector<std::string>(begin, end);
}

template<class ArrayType, class CallbackType, class AccumulatorType>
AccumulatorType reduce(const ArrayType &ary, CallbackType &&callback, AccumulatorType &&accumulator) {
  for (const auto &value : ary) {
    callback(accumulator, value);
  }
  return std::move(accumulator);
}

template<template<class, class, class...> class ObjectType, class KeyType, class ValueType, class... Types>
std::vector<KeyType> keys(const ObjectType<KeyType, ValueType, Types...> &object) {
  std::vector<KeyType> object_keys;
  for (const auto &key_value : object) {
    object_keys.emplace_back(key_value.first);
  }
  return object_keys;
}

template<class ArrayType, class CallbackType>
void permuteImpl(ArrayType &ary, size_t i, CallbackType &&callback) {
  if (i == ary.size()) {
    callback();
    return;
  }
  for (size_t j = i; j < ary.size(); ++j) {
    std::swap(ary[i], ary[j]);
    permuteImpl(ary, i + 1, std::forward<CallbackType>(callback));
    std::swap(ary[i], ary[j]);
  }
}

template<class IteratorType, class CallbackType>
void permute(IteratorType itr, IteratorType end, CallbackType &&callback) {
  if (itr == end) {
    callback();
    return;
  }
  auto &value = itr->second;
  ++itr;
  permuteImpl(value, 0, [&itr, &end, &callback] () {
    permute(itr, end, callback);
  });
}

int main() {
  const std::string code("0000 111 0101 111 0111 1000 0001 110 111 0110 001 0100 101 110 110 111 001 10010 101 10011 101");
  const std::unordered_map<char, int> chars({
    {'C', 1}, {'H', 1}, {'I', 1}, {'N', 1}, {'W', 1}, {'b', 1}, {'d', 3}, {'e', 4}, {'i', 3}, {'k', 1}, {'l', 1}, {'n', 2}, {'u', 1}
  });

  const auto tokens(split(code));
  const auto count(reduce(tokens, [] (std::unordered_map<std::string, int> &count, const std::string &token) {
    ++count[token];
  }, std::unordered_map<std::string, int>()));

  auto group(reduce(keys(chars), [&chars] (std::unordered_map<int, std::vector<char>> &group, char c) {
    auto itr = chars.find(c);
    if (itr == chars.end()) {
      return;
    }
    group[itr->second].emplace_back(c);
  }, std::unordered_map<int, std::vector<char>>()));
  std::unordered_map<std::string, char> dictionary;
  std::ofstream ofs("output.txt");
  permute(group.begin(), group.end(), [&tokens, &count, &group, &dictionary, &ofs] () {
    auto group_copy(group);
    for (const auto &pair : count) {
      auto &chars = group_copy[pair.second];
      dictionary[pair.first] = chars.back();
      chars.pop_back();
    }
    for (const auto &token : tokens) {
      ofs << dictionary[token];
    }
    ofs << std::endl;
  });

  return 0;
}
