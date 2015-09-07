/*
Copyright (c) 2015, Edward Haas
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.

* Neither the name of objmempool nor the names of its
  contributors may be used to endorse or promote products derived from
  this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

 * objmempool.h
 *
 */

#ifndef OBJMEMPOOL_H_
#define OBJMEMPOOL_H_

#undef new

#ifndef UNUSED
#define UNUSED(x) (void(x))
#endif

#include "rte/rte_ring.h"
#include "objmempool_container.h"
#include <typeinfo>

#define POWEROF2(x) ((((x)-1) & (x)) == 0)

template <typename OBJ_TYPE>
class Objmempool
{
    typedef rte_ring Free_list;
    typedef OBJ_TYPE obj_mem_slot;     // For debug purposes, the obj_mem_slot will include debug info and embed the object type.

protected:
    Objmempool() {};
    ~Objmempool() {};

public:
    static void * operator new (std::size_t size);

    static void* operator new  ( std::size_t size, const std::nothrow_t& tag);

    static void operator delete (void * ptr);

    static void operator delete  ( void* ptr, const std::nothrow_t& tag );

    /*
     *  Mempool container, implemented using a MP/MC ring queue.
     *  Must be created at the application global init stage.
     */
    static void mempool_create(std::size_t object_count);
    static void mempool_destroy();

    static std::size_t get_mempool_free_obj_count();
    static std::size_t get_mempool_size();

    static int show_mempool_cmd(int argc, const char **argv, char *buf, std::size_t buf_size);

private:

    static void round_up_to_a_powerof2(uint32_t & size);

    static rte_ring * new_free_list(std::string _name, unsigned int q_size, unsigned int type);

    static Free_list * free_list;
    static std::size_t obj_count;
    static obj_mem_slot * obj_memory_head;

    static __thread OBJ_TYPE * cache;		// NOTE: This is a TLS variable.
	
// Unsupported operators.
private:
    static void * operator new[] (std::size_t sz) { UNUSED(sz); return NULL; }
    static void * operator new[] (std::size_t sz, const std::nothrow_t& tag) { UNUSED(sz); return NULL; }
    static void operator delete[] (void * ptr) { UNUSED(ptr); }
    static void operator delete[] (void * ptr, const std::nothrow_t& tag) { UNUSED(ptr); }
};


/*****************************
 ** Implementation details  **
 *****************************/

template <typename OBJ_TYPE>
std::size_t Objmempool<OBJ_TYPE>::obj_count = 0;

template <typename OBJ_TYPE>
typename Objmempool<OBJ_TYPE>::Free_list * Objmempool<OBJ_TYPE>::free_list = NULL;

template <typename OBJ_TYPE>
OBJ_TYPE * Objmempool<OBJ_TYPE>::obj_memory_head = NULL;

template <typename OBJ_TYPE>
__thread OBJ_TYPE * Objmempool<OBJ_TYPE>::cache = NULL;


template <typename OBJ_TYPE>
void * Objmempool<OBJ_TYPE>::operator new (std::size_t size)
{
    UNUSED(size);
    void * obj_array[1];
    const unsigned int num = 1;
    unsigned int n = rte_ring_dequeue_burst(free_list, obj_array, num);
    if(unlikely(n <= 0))
        abort();

    void * obj = obj_array[0];
    return obj;
}

template <typename OBJ_TYPE>
void* Objmempool<OBJ_TYPE>::operator new  ( std::size_t size, const std::nothrow_t& tag)
{
    UNUSED(tag);
    return operator new(size);
}

template <typename OBJ_TYPE>
void Objmempool<OBJ_TYPE>::operator delete (void * ptr)
{
    const unsigned int num = 1;
    unsigned int n = rte_ring_enqueue_burst(free_list, &ptr, num);
    UNUSED(n);
}

template <typename OBJ_TYPE>
void Objmempool<OBJ_TYPE>::operator delete  ( void* ptr, const std::nothrow_t& tag )
{
    UNUSED(tag);
    operator delete(ptr);
}

/*
 *  Mempool container, implemented using a MP/MC ring queue.
 *  Must be created at the application global init stage.
 */
template <typename OBJ_TYPE>
void Objmempool<OBJ_TYPE>::mempool_create(std::size_t object_count)
{
    if(NULL == free_list)
    {
    	free_list = new_free_list("noname", object_count, 0);
        obj_count = object_count-1;

        void * mem = malloc(obj_count * sizeof(obj_mem_slot));
        obj_memory_head = static_cast<obj_mem_slot*>(mem);
        obj_mem_slot * obj = obj_memory_head;
        for (size_t i=0; i < obj_count; ++i, ++obj)
        {
            void * const vobj = static_cast<void*>(obj);
            rte_ring_enqueue_burst(free_list, &vobj, 1);
        }

        Objmempool_container::add(static_cast<uint8_t*>(mem),
                                  sizeof(obj_mem_slot),
                                  obj_count,
                                  show_mempool_cmd);
    }
}

template <typename OBJ_TYPE>
void Objmempool<OBJ_TYPE>::mempool_destroy()
{
    free(free_list);
    free(obj_memory_head);
    obj_count = 0;
    free_list = NULL;
}

template <typename OBJ_TYPE>
std::size_t Objmempool<OBJ_TYPE>::get_mempool_free_obj_count()
{
    if(NULL != free_list)
        return rte_ring_count(free_list);
    else
        return 0;
}

template <typename OBJ_TYPE>
std::size_t Objmempool<OBJ_TYPE>::get_mempool_size()
{
    return obj_count;
}

template <typename OBJ_TYPE>
int Objmempool<OBJ_TYPE>::show_mempool_cmd(int argc, const char **argv, char *buf, std::size_t buf_size)
{
    UNUSED(argc);
    UNUSED(argv);

    char * const buf_base = buf;

    int ch_num = snprintf(buf, buf_size, "Mempool [%s]: %lu / %lu (%lu%% usage).\n",
                          typeid(OBJ_TYPE).name(),
                          get_mempool_free_obj_count(),
                          get_mempool_size(),
                          100 - 100 * get_mempool_free_obj_count() / get_mempool_size());
    buf += ch_num;
    buf_size -= ch_num;

    return (buf - buf_base);
}


// Private implementations

template <typename OBJ_TYPE>
void Objmempool<OBJ_TYPE>::round_up_to_a_powerof2(uint32_t & size)
{
    // Assumes 32 bit size.
    --size;
    size |= size >> 1;
    size |= size >> 2;
    size |= size >> 4;
    size |= size >> 8;
    size |= size >> 16;
    ++size;
}

template <typename OBJ_TYPE>
rte_ring * Objmempool<OBJ_TYPE>::new_free_list(std::string _name, unsigned int q_size, unsigned int type)
{
    if(! POWEROF2(q_size))
        round_up_to_a_powerof2(q_size);

    rte_ring * ring = rte_ring_create(_name.c_str(), q_size, SOCKET_ID_ANY, type);
    if(NULL == ring)
        abort();

    return ring;
}

#endif /* OBJMEMPOOL_H_ */
