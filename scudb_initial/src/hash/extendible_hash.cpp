#include <cassert>
#include <functional>
#include <list>
#include <bitset>
#include <iostream>
#include "hash/extendible_hash.h"
#include "page/page.h"
namespace scudb {

/*
 * constructor
 * array_size: fixed array size for each bucket
 */
template <typename K, typename V>
ExtendibleHash<K, V>::ExtendibleHash(size_t size): 
bucket_size(size), bucket_num(0),pair_num(0), depth(0)
{
  bucket_table.emplace_back(new Bucket(0,0));
  bucket_num = 1;
}

/*
 * helper function to calculate the hashing address of input key
 */
template <typename K, typename V>
size_t ExtendibleHash<K, V>::HashKey(const K &key) {
  return std::hash<K>()(key);
}

/*
 * helper function to return global depth of hash table
 * NOTE: you must implement this function in order to pass test
 */
template <typename K, typename V>
int ExtendibleHash<K, V>::GetGlobalDepth() const {
  std::lock_guard<std::mutex> lock(mutex_);
  return depth;
}

/*
 * helper function to return local depth of one specific bucket
 * NOTE: you must implement this function in order to pass test
 */
template <typename K, typename V>
int ExtendibleHash<K, V>::GetLocalDepth(int bucket_id) const {
  std::lock_guard<std::mutex> lock(mutex_);
  if(bucket_table[bucket_id]) {
        return bucket_table[bucket_id]->depth;
  }
  return -1;  // no bucket
}

/*
 * helper function to return current number of bucket in hash table
 */
template <typename K, typename V>
int ExtendibleHash<K, V>::GetNumBuckets() const {
  std::lock_guard<std::mutex> lock(mutex_);
  return bucket_num;
}

/*
 * lookup function to find value associate with input key
 */
template <typename K, typename V>
bool ExtendibleHash<K, V>::Find(const K &key, V &value) {
  std::lock_guard<std::mutex> lock(mutex_);
  size_t position = HashKey(key) & ((1 << depth) - 1);  // calculate position

  if(bucket_table[position]) {
      if(bucket_table[position]->items.find(key) != bucket_table[position]->items.end()) {
          value = bucket_table[position]->items[key];
          return true;
      }
  }
  return false;
}

/*
 * delete <key,value> entry in hash table
 * Shrink & Combination is not required for this project
 */
template <typename K, typename V>
bool ExtendibleHash<K, V>::Remove(const K &key) {
  std::lock_guard<std::mutex> lock(mutex_);
  size_t position = HashKey(key) & ((1 << depth) - 1);
  size_t cnt = 0;
  
  if(bucket_table[position]) {
    auto bucket_tmp = bucket_table[position];
    cnt = bucket_tmp->items.erase(key);
    pair_num -= cnt;
  }
  return cnt != 0;
}

/*
 * insert <key,value> entry in hash table
 * Split & Redistribute bucket when there is overflow and if necessary increase
 * global depth
 */
template <typename K, typename V>
void ExtendibleHash<K, V>::Insert(const K &key, const V &value) {
  std::lock_guard<std::mutex> lock(mutex_);
  size_t bucket_id = HashKey(key) & ((1 << depth) - 1);

  // find the position, if null, build one bucket.
  if(bucket_table[bucket_id] == nullptr) {
    bucket_table[bucket_id] = std::make_shared<Bucket>(bucket_id, depth);
    ++bucket_num;
  }
  auto bucket = bucket_table[bucket_id];

  // if it is not null, just cover.
  if(bucket->items.find(key) != bucket->items.end()) {
    bucket->items[key] = value;
    return;
  }

  // insert the pair
  bucket->items.insert({key, value});
  ++pair_num;
  // overflow, need to reassign
  if(bucket->items.size() > bucket_size) {
    // record the old index and depth
    auto old_index = bucket->id;
    auto old_depth = bucket->depth;
    std::shared_ptr<Bucket> new_bucket = divide(bucket);
    // if null(overflow),return old.
    if(new_bucket == nullptr) {
      bucket->depth = old_depth;
      return;
    }
    // if the local depth > globle depth, extend the table
    if(bucket->depth > depth) {
      auto size = bucket_table.size();
      auto factor = (1 << (bucket->depth - depth));
      depth = bucket->depth;
      bucket_table.resize(bucket_table.size()*factor);
      bucket_table[bucket->id] = bucket;
      bucket_table[new_bucket->id] = new_bucket;
      // iterate table
      for(size_t i = 0; i < size; ++i) {
        if(bucket_table[i]) {
          if(i < bucket_table[i]->id){
            bucket_table[i].reset();
          } 
          else {
            auto k = 1 << bucket_table[i]->depth;
            for(size_t j = i + k; j < bucket_table.size(); j += k) {
              bucket_table[j] = bucket_table[i];
            }
          }
        }
      }
    } 
    else 
    {
      //if the local depth <= globle depth
      for (size_t i = old_index; i < bucket_table.size(); i += (1 << old_depth)) {
        bucket_table[i].reset();
      }
      bucket_table[bucket->id] = bucket;
      bucket_table[new_bucket->id] = new_bucket;
      auto k = 1 << bucket->depth;
      for (size_t i = bucket->id + k; i < bucket_table.size(); i += k) {
        bucket_table[i] = bucket;
      }
      for (size_t i = new_bucket->id + k; i < bucket_table.size(); i += k) {
        bucket_table[i] = new_bucket;
      }
    }
  }
}
/*
 * divide <key,value> 
 * divide the bucket when there is overflow and if necessary increase
 */
template <typename K, typename V>
std::shared_ptr<typename ExtendibleHash<K, V>::Bucket>ExtendibleHash<K, V>::divide(std::shared_ptr<Bucket> &buc) 
{
    // create a new bucket 
    auto res = std::make_shared<Bucket>(0, buc->depth);
    while(res->items.empty()) {
        // depth+=1
        buc->depth++;
        res->depth++;
        // implement the allocate of the bucket
        for(auto it = buc->items.begin(); it != buc->items.end();) {
            if (HashKey(it->first) & (1 << (buc->depth - 1))) {
                res->items.insert(*it);
                res->id = HashKey(it->first) & ((1 << buc->depth) - 1);
                it = buc->items.erase(it);
            } else {
                ++it;
            }
        }
        // the depth is not enouth, need to iterate
        if(buc->items.empty()) {
            buc->items.swap(res->items);
            buc->id = res->id;
        }
    }
    ++bucket_num;
    return res;
}
template class ExtendibleHash<page_id_t, Page *>;
template class ExtendibleHash<Page *, std::list<Page *>::iterator>;
// test purpose
template class ExtendibleHash<int, std::string>;
template class ExtendibleHash<int, std::list<int>::iterator>;
template class ExtendibleHash<int, int>;
} // namespace scudb
