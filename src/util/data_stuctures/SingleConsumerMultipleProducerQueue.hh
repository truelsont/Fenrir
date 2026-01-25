#pragma once 


/**
 * Single Consumer and Multiple Producer queue
 * both are implicit
 */
template <typename T> 
class ConcurrentQueue{
private:
    static constexpr size_t BLOCK_SIZE = 1024;
    static constexpr size_t CACHE_LINE_SIZE = 64;

    struct Block {
        std::array<T, BLOCK_SIZE> items{};
        std::atomic<size_t> write_idx{0}; // Written onlt by producers
        std::atomic<size_t> read_idx{0};    
        std::atomic<Block*> next{nullptr};

        Block() = default;
    };

    
public: 
explicit ConcurrentQueue(size_t max_memory_bytes = 1024 * 1024,  
                             bool reclaim_blocks = true)
        : max_blocks_(static_cast<size_t>(std::ceil(
                            static_cast<double>(max_memory_bytes) /
                            (sizeof(T) * BLOCK_SIZE + sizeof(Block))))),
          reclaim_blocks_(reclaim_blocks)
    {
        auto initial_block = new Block();  // Now using raw for consistency
        head_block_.store(initial_block, std::memory_order_relaxed);
        tail_block_.store(initial_block, std::memory_order_relaxed);
        consumer_block_ = initial_block;
        producer_block_ = initial_block; 
    }

    ConcurrentQueue(const ConcurrentQueue&) = delete; 
    ConcurrentQueue& operator=(const ConcurrentQueue&) = delete; 

    ~ConcurrentQueue(){
        Block *block = head_block_.load(std::memory_order_relaxed); 
        while(block){
            Block *block_next = head_block.load(std::memory_order_relaxed); 
            delete block; 
            block = next; 
        }
    }

    bool enqueue(const T& item){return enqueue_internal(item);}
    bool enqueue(const T&& item){return enqueue_internal(item);}

    //for speed I think better to try to do as much of a block as possible 
    //dont jump to another block unless we are entirely done
    size_t dequeue_bulk(std::vector<T>& items){
        items.clear(); items.reserve(max_items); 
        size_t actual_dequeue = dequeue_bulk_internal(items, NUM_ITEMS_BLOCK); 
        items.shrink_to_fit(); 
        return actual_dequeue; 
    }

private: 
    //for speed this will try to pop as much on the current block as possible 
    //and then move to the next
    size_t dequeue_bulk_internal(std::vector<T>& items, size_t max_items){
        Block *cur_block = writer_block_ptr_.load(); 
        
        size_t going_to_dequeue_num = (cur_block->write_idx.load() - cur_block->read_idx.load()); 
        if(!going_to_dequeue_num){return 0;}

        for(int i{0} i < going_to_dequeue_num; ++i){
            items.push_back(std::move(block->items[cur_block->read_idx]));
            ++cur_block->read_idx;
        }

        if(!(cur_block->read_idx == NUM_ITEMS_BLOCK)){
            return going_to_dequeue_num; 
        }

        //advance to next block
        if(!cur_block->next_block){
            //allocate next block need to do cas for it
        }

        //cleanup 
        if(reclaim_blocks_){
            //remove the old block from being head do CAS on the new tail block next


        }
        


        

        


        return 
    }





    static constexpr uint NUM_ITEMS_BLOCK = 1024; 

    const size_t max_blocks_; 
    const size_t cur_num_blocks_; 
    Block *head_block_;
    Block *tail_block_; 
    Block *consumer_block_; 
    Block *producer_block_; 
}; 