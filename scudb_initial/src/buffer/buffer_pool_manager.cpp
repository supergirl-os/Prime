#include "buffer/buffer_pool_manager.h"

namespace scudb {

/*
 * BufferPoolManager Constructor
 * When log_manager is nullptr, logging is disabled (for test purpose)
 * WARNING: Do Not Edit This Function
 */
BufferPoolManager::BufferPoolManager(size_t pool_size,
                                                 DiskManager *disk_manager,
                                                 LogManager *log_manager)
    : pool_size_(pool_size), disk_manager_(disk_manager),
      log_manager_(log_manager) {
  // a consecutive memory space for buffer pool
  pages_ = new Page[pool_size_];
  page_table_ = new ExtendibleHash<page_id_t, Page *>(BUCKET_SIZE);
  replacer_ = new LRUReplacer<Page *>;
  free_list_ = new std::list<Page *>;

  // put all the pages into free list
  for (size_t i = 0; i < pool_size_; ++i) {
    free_list_->push_back(&pages_[i]);
  }
}

/*
 * BufferPoolManager Deconstructor
 * WARNING: Do Not Edit This Function
 */
BufferPoolManager::~BufferPoolManager() {
  delete[] pages_;
  delete page_table_;
  delete replacer_;
  delete free_list_;
}

/**
 * 1. search hash table.
 *  1.1 if exist, pin the page and return immediately
 *  1.2 if no exist, find a replacement entry from either free list or lru
 *      replacer. (NOTE: always find from free list first)
 * 2. If the entry chosen for replacement is dirty, write it back to disk.
 * 3. Delete the entry for the old page from the hash table and insert an
 * entry for the new page.
 * 4. Update page metadata, read page content from disk file and return page
 * pointer
 */
Page *BufferPoolManager::FetchPage(page_id_t page_id) { 
  assert(page_id != INVALID_PAGE_ID);
	std::lock_guard<std::mutex> lock(latch_);
	Page *dst_page = nullptr;
	if (page_table_->Find(page_id, dst_page))
	{
		//pin the page
		++dst_page->pin_count_;
		replacer_->Erase(dst_page);
		return dst_page;
	}
	else
	{
    // find a replacement entry from either free list or lru replacer
		if (!free_list_->empty())
		{
			dst_page = free_list_->front();
			free_list_->pop_front();
		}
		else
		{
			if (!replacer_->Victim(dst_page))
			{
				return nullptr;
			}
		}
	}
	assert(dst_page->pin_count_ == 0);
	if (dst_page->is_dirty_)
	{
    // If the entry chosen for replacement is dirty, write it back to disk.
		disk_manager_->WritePage(dst_page->page_id_, dst_page->GetData());
	}
	// delete the entry for old page from the hash table
	page_table_->Remove(dst_page->page_id_);

	// insert an entry for the new page
	page_table_->Insert(page_id, dst_page);

	// Update page metadata
	dst_page->page_id_ = page_id;
	dst_page->is_dirty_ = false;
	dst_page->pin_count_ = 1;
  // read page content from disk file
	disk_manager_->ReadPage(page_id, dst_page->GetData());
  // return page pointer
	return dst_page;
}
/*
 * Implementation of unpin page
 * if pin_count>0, decrement it and if it becomes zero, put it back to
 * replacer if pin_count<=0 before this call, return false. is_dirty: set the
 * dirty flag of this page
 */
bool BufferPoolManager::UnpinPage(page_id_t page_id, bool is_dirty) {
  std::lock_guard<std::mutex> lock(latch_);

	Page *dst_page = nullptr;
	if (!page_table_->Find(page_id, dst_page))
	{
		return false;
	}
	else
	{
		if (dst_page->pin_count_ > 0)
		{
			if (--dst_page->pin_count_ == 0)
			{
        // put it back to replacer
				replacer_->Insert(dst_page);
			}
		}
		else
		{
			return false;
		}

		if (is_dirty)
		{
      // set the dirty flag of this page
			dst_page->is_dirty_ = true;
		}
		return true;
	}
}

/*
 * Used to flush a particular page of the buffer pool to disk. Should call the
 * write_page method of the disk manager
 * if page is not found in page table, return false
 * NOTE: make sure page_id != INVALID_PAGE_ID
 */
bool BufferPoolManager::FlushPage(page_id_t page_id) { 
	std::lock_guard<std::mutex> lock(latch_);
	if (page_id == INVALID_PAGE_ID)
    // make sure page_id != INVALID_PAGE_ID
		return false;
	Page *dst_page = nullptr;
	if (page_table_->Find(page_id, dst_page))
	{
    // page is not found in page table
		disk_manager_->WritePage(page_id, dst_page->GetData());
		return true;
	}
	return false;
}

/**
 * User should call this method for deleting a page. This routine will call
 * disk manager to deallocate the page. First, if page is found within page
 * table, buffer pool manager should be reponsible for removing this entry out
 * of page table, dst_pageeting page metadata and adding back to free list. Second,
 * call disk manager's DeallocatePage() method to delete from disk file. If
 * the page is found within page table, but pin_count != 0, return false
 */
bool BufferPoolManager::DeletePage(page_id_t page_id) { 
  std::lock_guard<std::mutex> lock(latch_);
	Page *dst_page = nullptr;
	if(page_table_->Find(page_id, dst_page))
	{
    //  page is found within page table
		page_table_->Remove(page_id);// removing this entry out of page table
    //dst_pageeting page metadata 
		dst_page->page_id_ = INVALID_PAGE_ID;
		dst_page->is_dirty_ = false;
		replacer_->Erase(dst_page);
    // adding back to free list
    free_list_->push_back(dst_page);
    // call disk manager's DeallocatePage() method to delete from disk file
		disk_manager_->DeallocatePage(page_id);

		return true;
	}
  // the page is found within page table, but pin_count != 0, return false
	return false;
 }

/**
 * User should call this method if needs to create a new page. This routine
 * will call disk manager to allocate a page.
 * Buffer pool manager should be dst_pageponsible to choose a victim page either
 * from free list or lru replacer(NOTE: always choose from free list first),
 * update new page's metadata, zero out memory and add cordst_pageponding entry
 * into page table. return nullptr if all the pages in pool are pinned
 */
Page *BufferPoolManager::NewPage(page_id_t &page_id) { 
  std::lock_guard<std::mutex> lock(latch_);

	Page *dst_page = nullptr;
	if(!free_list_->empty())
	{
    // choose from free list first
		dst_page = free_list_->front();
		free_list_->pop_front();
	}
	else
	{
		if(!replacer_->Victim(dst_page))
		{
      // all the pages in pool are pinned
			return nullptr;
		}
	}
	page_id = disk_manager_->AllocatePage();
	if(dst_page->is_dirty_)
	{
		disk_manager_->WritePage(dst_page->page_id_, dst_page->GetData());
	}
  page_table_->Remove(dst_page->page_id_);
  // add cordst_pageponding entry into page table
	page_table_->Insert(page_id, dst_page);   
  // update new page's metadata
	dst_page->page_id_ = page_id;
	dst_page->is_dirty_ = false;
	dst_page->pin_count_ = 1;
	dst_page->ResetMemory();  //zero out memory
	return dst_page;
}
} // namespace scudb
