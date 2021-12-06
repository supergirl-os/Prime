/**
 * lru_replacer.h
 *
 * Functionality: The buffer pool manager must maintain a LRU list to collect
 * all the pages that are unpinned and ready to be swapped. The simplest way to
 * implement LRU is a FIFO queue, but remember to dequeue or enqueue pages when
 * a page changes from unpinned to pinned, or vice-versa.
 */

#pragma once
#include <map>
#include "buffer/replacer.h"
#include "hash/extendible_hash.h"
namespace scudb {
template <typename T> class LRUReplacer : public Replacer<T> {
public:
  // do not change public interface
  LRUReplacer();

  ~LRUReplacer();

  void Insert(const T &value);

  bool Victim(T &value);

  bool Erase(const T &value);

  size_t Size();

private:
  // add your member variables here
  // I choose to use a built-in STL container--std::map
  struct node {
    node() = default;
    explicit node(T d, node *p = nullptr) : data(d), pre(p) {}
    T data;
    std::shared_ptr<node> pre;
    std::shared_ptr<node> next;
  };
  std::shared_ptr<node> head;
  std::shared_ptr<node> tail;
  int size_ = 0;
  std::mutex mutex_;
  std::map<T, std::shared_ptr<node>> bucket_table; // the table for buckets
};

} // namespace scudb
