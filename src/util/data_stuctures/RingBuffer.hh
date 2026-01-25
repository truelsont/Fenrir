#include <atomic>

template <typename Item>
class RingBuffer{
public:
    //going to do this to avoid problems
    explicit RingBuffer(constexpr size_t num_of_items):
        buffer_item_size_{num_of_items}, 
        buffer_[num_of_items]
    {
        static_assert(sizeof(Item) * num_of_items <= 1024 * 1024, 
            "Size of ring buffer needs to be less than 1 MB"); 
    }; 

    bool Write(Item&& item){
        static_assert(!std::is_lvalue_reference_v<T>, 
            "RingBuffer::Write requires rvalue only"); 
        size_t w_idx{std::atomic.load(write_idx_, std::memory_order_seq_cst);
        size_t r_idx{std::atomic.load(read_idx_, std::memory_order_acquire)};
        if (w_idx + 1 % buffer_item_size_){return false;} //full

        write_idx.load(w_idx); 
        return false; 
    }

    bool Read(Item& item){

        return false; 
    }

    size_t GetMaxItemSize(){return buffer_item_size;}
    size_t GetMaxMemSize(){return buffer_item_size * sizeof(Item);}

private: 
    //align them for cache size so no false sharing issues? 
    std::atomic<size_t> read_idx_{0}; 
    std::atomic<size_t> write_idx_{0}; 
    const size_t buffer_item_size_;
    std::unique_ptr<Item[]> buffer_; 
}; 