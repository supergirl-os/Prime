/**
 * LRU implementation
 */
#include "buffer/lru_replacer.h"
#include "page/page.h"

namespace scudb {

template <typename T> LRUReplacer<T>::LRUReplacer() {}

template <typename T> LRUReplacer<T>::~LRUReplacer() {}

/*
 * Insert value into LRU
 */
template <typename T> void LRUReplacer<T>::Insert(const T &value) {
    std::lock_guard<std::mutex> guard(mutex_);
    auto index = bucket_table.find(value);
    if (index != bucket_table.end()) {
        auto ptr =index->second;
    if (ptr == head && ptr == tail) {
      // null
        head = nullptr;
        tail = nullptr;
    } 
    else if (ptr == head) {
        head = ptr->next;
        ptr->next->pre = nullptr; 
    } 
    else if (ptr == tail) {
        tail = ptr->pre;
        ptr->pre->next = nullptr;
    } 
    else {
        ptr->pre->next = ptr->next;
        ptr->next->pre = ptr->pre;
    }
    ptr->pre = nullptr;
    ptr->next = nullptr;

    bucket_table.erase(value);  
    size_--;
    }
   
    auto new_node = std::make_shared<node>(value);
    if (new_node == nullptr) {
        return;
    }
    new_node->pre = nullptr;
    new_node->next = head;
    if (head != nullptr) {
        head->pre = new_node;
    }
    head = new_node;
    if (tail == nullptr) {
        tail = new_node;
    }
    bucket_table[new_node->data] = new_node;
    size_++;
    
}


/* If LRU is non-empty, pop the head member from LRU to argument "value", and
 * return true. If LRU is empty, return false
 */
template <typename T> bool LRUReplacer<T>::Victim(T &value) {
    std::lock_guard<std::mutex> guard(mutex_);

    if (size_ == 0) {
        return false;
    }
    if (head == tail) {
        value = head->data;
        head = nullptr;
        tail = nullptr;
        bucket_table.erase(value);
        size_--;
        return true;
    }
    value = tail->data;
    auto discard = tail;
    discard->pre->next = nullptr;
    tail = discard->pre;
    discard->pre = nullptr;
    bucket_table.erase(value);
    size_--;
    return true;
}

/*
 * Remove value from LRU. If removal is successful, return true, otherwise
 * return false
 */
template <typename T> bool LRUReplacer<T>::Erase(const T &value) {
    std::lock_guard<std::mutex> guard(mutex_);
    auto index = bucket_table.find(value);
    if (index == bucket_table.end()) {
        return false;
    }
    auto ptr =index->second;
    if (ptr == head && ptr == tail) {
      // null
        head = nullptr;
        tail = nullptr;
    } 
    else if (ptr == head) {
        head = ptr->next;
        ptr->next->pre = nullptr; 
    } 
    else if (ptr == tail) {
        tail = ptr->pre;
        ptr->pre->next = nullptr;
    } 
    else {
        ptr->pre->next = ptr->next;
        ptr->next->pre = ptr->pre;
    }
    ptr->pre = nullptr;
    ptr->next = nullptr;

    bucket_table.erase(value);  
    size_--;
    return true;
}

template <typename T> size_t LRUReplacer<T>::Size() { return size_; }

template class LRUReplacer<Page *>;
// test only
template class LRUReplacer<int>;

} // namespace scudb
