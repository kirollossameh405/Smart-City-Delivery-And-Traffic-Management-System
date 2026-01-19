#ifndef HASH_TABLE_HPP
#define HASH_TABLE_HPP

#include <vector>
#include <list>
#include <functional>
#include <optional>
#include <string>
#include "types.hpp"

using namespace std;

template<typename K, typename V>
class HashTable {
private:
    static constexpr size_t DEFAULT_SIZE = 101;
    vector<list<pair<K, V>>> table;
    size_t num_elements = 0;
    function<size_t(const K&)> hash_func;

    size_t get_index(const K& key) const;
    void rehash();

public:
    HashTable();
    HashTable(size_t initial_size, function<size_t(const K&)> hasher);

    void insert(const K& key, const V& value);
    optional<V> find(const K& key) const;
    bool remove(const K& key);
    size_t size() const;
    bool empty() const;
};

template<> size_t HashTable<int, Location>::get_index(const int& key) const;
template<> size_t HashTable<int, Vehicle>::get_index(const int& key) const;

#endif