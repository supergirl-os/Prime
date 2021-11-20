/*
 * extendible_hash.h : implementation of in-memory hash table using extendible
 * hashing
 *
 * Functionality: The buffer pool manager must maintain a page table to be able
 * to quickly map a PageId to its corresponding memory location; or alternately
 * report that the PageId does not match any currently-buffered page.
 */

#pragma once
#include <cstdlib>
#include <vector>
#include <string>
#include <mutex>
#include <map>
#include <memory>
#include "hash/hash_table.h"

namespace scudb {

template <typename K, typename V>
class ExtendibleHash : public HashTable<K, V> {
public:
  // constructor
  ExtendibleHash(size_t size);
  // helper function to generate hash addressing
  size_t HashKey(const K &key);
  // helper function to get global & local depth
  int GetGlobalDepth() const;
  int GetLocalDepth(int bucket_id) const;
  int GetNumBuckets() const;
  // lookup and modifier
  bool Find(const K &key, V &value) override;
  bool Remove(const K &key) override;
  void Insert(const K &key, const V &value) override;
  // get the number of key value pairs in the hash table
  size_t pair_size() const { return pair_num; }

private:
  // add your own member variables here
  // add data structure-the bucket, using the disordered map.
    struct Bucket {
      Bucket() = default; // default construction function
      explicit Bucket(size_t i, int d) : id(i), depth(d) {}   // explicit construction
      std::map<K, V> items;          // key-value pairs
      size_t id = 0;                 // id of Bucket
      int depth = 0;                 // local depth
  };
    const size_t bucket_size;     // the number of the element in one bucket
    mutable std::mutex mutex_;    // using mutable to change variable
    int bucket_num;               // the number of the buckets that are used
    size_t pair_num;              // the number of key value pairs in the hash table
    int depth;                    // the globle depth of the bucket
    std::vector<std::shared_ptr<Bucket>> bucket_table;    // the table of the bucket
    std::shared_ptr<Bucket> divide(std::shared_ptr<Bucket> &); // divide into new buckets

};
} // namespace scudb
