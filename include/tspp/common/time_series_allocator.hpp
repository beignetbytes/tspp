#pragma once
#ifndef time_series_allocator_hpp
#define time_series_allocator_hpp

#include <memory>

namespace tspp {

    template <class T, std::size_t growsize = 1024>
    class memory_pool {
        struct block {
            block* next;
        };

        class buffer {
            static const std::size_t blocksize = sizeof(T) > sizeof(block) ? sizeof(T) :
                                                                             sizeof(block);
            uint8_t data[blocksize * growsize];

          public:
            buffer* const next;

            buffer(buffer* next) : next(next) {}

            T* getblock(std::size_t index) {
                return reinterpret_cast<T*>(&data[blocksize * index]);
            }
        };

        block* first_freeblock = nullptr;
        buffer* firstbuffer = nullptr;
        std::size_t bufferedblocks = growsize;


      public:
        memory_pool() = default;
        memory_pool(memory_pool&& memory_pool) = delete;
        memory_pool(const memory_pool& memory_pool) = delete;
        memory_pool operator=(memory_pool&& memory_pool) = delete;
        memory_pool operator=(const memory_pool& memory_pool) = delete;

        ~memory_pool() {
            while (firstbuffer) {
                buffer* buffer = firstbuffer;
                firstbuffer = buffer->next;
                delete buffer;
            }
        }

        T* allocate() {
            if (first_freeblock) {
                block* block = first_freeblock;
                first_freeblock = block->next;
                return reinterpret_cast<T*>(block);
            }

            if (bufferedblocks >= growsize) {
                firstbuffer = new buffer(firstbuffer);
                bufferedblocks = 0;
            }

            return firstbuffer->getblock(bufferedblocks++);
        }

        void deallocate(T* pointer) {
            block* bl = reinterpret_cast<block*>(pointer);
            bl->next = first_freeblock;
            first_freeblock = bl;
        }
    };

    template <class T, std::size_t growsize = 1024>
    class allocator : private memory_pool<T, growsize> {

        using std_alloc = typename std::allocator<T>;
#ifdef _WIN32
        allocator* copyallocator = nullptr;
        std_alloc* rebindallocator = nullptr;
#endif

      public:
        using size_type = typename std::allocator_traits<std_alloc>::size_type;
        using difference_type = typename std::allocator_traits<std_alloc>::difference_type;
        using pointer = typename std::allocator_traits<std_alloc>::pointer;
        using const_pointer = typename std::allocator_traits<std_alloc>::const_pointer;
        using reference = T&;
        using const_reference = const T&;
        using value_type = typename std::allocator_traits<std_alloc>::value_type;

        template <class U>
        struct rebind {
            typedef allocator<U, growsize> other;
        };

#ifdef _WIN32
        allocator() = default;

        allocator(allocator& allocator) : copyallocator(&allocator) {}
        //allocator(const allocator& alloc) : copyallocator(const_cast<allocator*>((&alloc))) {} //added copy xtor

        //allocator(allocator&& src) // added move xtor to work with tsl since that tries to do some swaping propogation magic
        //{
        //    if (this != &src) {
        //        copyallocator = src.copyallocator;
        //        rebindallocator = src.rebindallocator;
        //    }
        //}; 

        //allocator& operator=(allocator &&src) { //added move assign to work with tsl since that tries to do some swaping propogation magic
        //    if (this != &src) {
        //        copyallocator = src.copyallocator;
        //        rebindallocator = src.rebindallocator;
        //    }
        //    return *this;
        //};


        template <class U>
        allocator(const allocator<U, growsize>& other) {
            if (!std::is_same<T, U>::value)
                rebindallocator = new std_alloc();
        }

        ~allocator() { delete rebindallocator; }
#endif
        
        pointer allocate(size_type n, const void* hint = 0) {
#ifdef _WIN32
            if (copyallocator)
                return copyallocator->allocate(n, hint);

            if (rebindallocator)
                return rebindallocator->allocate(n, hint);
#endif

            if (n != 1 || hint) // was
                throw std::bad_alloc();

            return memory_pool<T, growsize>::allocate();
        }

        void deallocate(pointer p, size_type n) {
#ifdef _WIN32
            if (copyallocator) {
                copyallocator->deallocate(p, n);
                return;
            }

            if (rebindallocator) {
                rebindallocator->deallocate(p, n);
                return;
            }
#endif

            memory_pool<T, growsize>::deallocate(p);
        }

        //void construct(pointer p, const_reference val) { new (p) T(val); }

        void construct(pointer p, reference val) { new (p) T(val); } //added to get rid of cosnt req

        void destroy(pointer p) { p->~T(); }
    };

}

#endif
