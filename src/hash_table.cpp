#include "../include/hash_table.hpp"
#include <utility>

template<typename K, typename V>
HashTable<K, V>::HashTable() : table(DEFAULT_SIZE), hash_func([](const K& k){ return hash<K>{}(k); }) {}

template<typename K, typename V>
HashTable<K, V>::HashTable(size_t initial_size, function<size_t(const K&)> hasher)
    : table(initial_size), hash_func(hasher) {}

template<typename K, typename V>
void HashTable<K, V>::insert(const K& key, V value) {
    size_t idx = get_index(key);
    auto& chain = table[idx];
    for (auto& p : chain) {
        if (p.first == key) {
            p.second = move(value);
            return;
        }
    }
    chain.emplace_back(key, move(value));
    ++num_elements;
    if (num_elements > table.size() * 0.7) rehash();
}

template<typename K, typename V>
optional<V*> HashTable<K, V>::find(const K& key) {
    size_t idx = get_index(key);
    auto& chain = table[idx];
    for (auto& p : chain) {
        if (p.first == key) return &p.second;
    }
    return nullopt;
}

template<typename K, typename V>
optional<const V*> HashTable<K, V>::find(const K& key) const {
    size_t idx = get_index(key);
    const auto& chain = table[idx];
    for (const auto& p : chain) {
        if (p.first == key) return &p.second;
    }
    return nullopt;
}

template<typename K, typename V>
bool HashTable<K, V>::remove(const K& key) {
    size_t idx = get_index(key);
    auto& chain = table[idx];
    for (auto it = chain.begin(); it != chain.end(); ++it) {
        if (it->first == key) {
            chain.erase(it);
            --num_elements;
            return true;
        }
    }
    return false;
}

template<typename K, typename V>
size_t HashTable<K, V>::size() const { 
    return num_elements; 
}

template<typename K, typename V>
bool HashTable<K, V>::empty() const { 
    return num_elements == 0; 
}

template<typename K, typename V>
void HashTable<K, V>::rehash() {
    size_t new_size = table.size() * 2 + 1;
    vector<list<pair<K, V>>> new_table(new_size);
    for (const auto& chain : table) {
        for (const auto& p : chain) {
            size_t new_idx = hash_func(p.first) % new_size;
            new_table[new_idx].push_back(p);
        }
    }
    table = move(new_table);
}

template<typename K, typename V>
size_t HashTable<K, V>::get_index(const K& key) const {
    return hash_func(key) % table.size();
}

template class HashTable<int, Location>;
template class HashTable<int, Vehicle>;
template class HashTable<int, Delivery>;