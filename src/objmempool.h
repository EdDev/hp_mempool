/*
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

#define POWEROF2(x) ((((x)-1) & (x)) == 0)

template <typename OBJ_TYPE>
class Objmempool
{
    typedef rte_ring Free_list;

protected:
    Objmempool() {};
    ~Objmempool() {};

public:
    static void * operator new (std::size_t size)
    {
        UNUSED(size);
        void * obj_array[1];
        const unsigned int num = 1;
        unsigned int n = rte_ring_dequeue_burst(free_list, obj_array, num);
        UNUSED(n);

        void * obj = obj_array[0];
        return obj;
    }

    static void* operator new  ( std::size_t size, const std::nothrow_t& tag)
    {
        UNUSED(tag);
    	return operator new(size);
    }

    static void operator delete (void * m)
    {
    	const unsigned int num = 1;
    	unsigned int n = rte_ring_enqueue_burst(free_list, &m, num);
    	UNUSED(n);
    }

    static void operator delete  ( void* ptr, const std::nothrow_t& tag )
    {
        UNUSED(tag);
    	operator delete(ptr);
    }

    /*
     *  Mempool container, implemented using a MP/MC ring queue.
     *  Must be created at the application global init stage.
     */
    static void mempool_create(std::size_t object_count)
    {
        if(NULL == free_list)
        {
        	free_list = new_free_list("noname", object_count, 0);
            obj_count = object_count-1;

            for (size_t i=0; i < obj_count; ++i)
            {
                OBJ_TYPE * const obj = ::new OBJ_TYPE();
                void * const vobj = static_cast<void*>(obj);
                rte_ring_enqueue_burst(free_list, &vobj, 1);
            }
        }
    }

    static void mempool_destroy()
    {
        for (size_t i=0; i < obj_count; ++i)
        {
            void * obj_array[1];
            const unsigned int num = 1;
            rte_ring_dequeue_burst(free_list, obj_array, num);

            OBJ_TYPE * obj = static_cast<OBJ_TYPE*>(obj_array[0]);
            ::delete obj;
        }

        free(free_list);
        obj_count = 0;
        free_list = NULL;
    }

    static size_t get_mempool_size()
    {
        if(NULL != free_list)
            return rte_ring_count(free_list);
        else
            return 0;
    }

private:

    static void round_up_to_a_powerof2(uint32_t & size)
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

    static rte_ring * new_free_list(std::string _name, unsigned int q_size, unsigned int type)
	{
        if(! POWEROF2(q_size))
            round_up_to_a_powerof2(q_size);

        rte_ring * ring = rte_ring_create(_name.c_str(), q_size, SOCKET_ID_ANY, type);
        if(NULL == ring)
            abort();

        return ring;
	}

    static Free_list * free_list;
    static size_t obj_count;

    static __thread OBJ_TYPE * cache;		// NOTE: This is a TLS variable.
	
// Unsupported operators.
private:
    static void * operator new[] (std::size_t count)
    {
        UNUSED(count);
        return NULL;
    }

    static void operator delete[] (void * m)
    {
        UNUSED(m);
    }
};

template <typename OBJ_TYPE>
size_t Objmempool<OBJ_TYPE>::obj_count = 0;

template <typename OBJ_TYPE>
typename Objmempool<OBJ_TYPE>::Free_list * Objmempool<OBJ_TYPE>::free_list = NULL;

template <typename OBJ_TYPE>
__thread OBJ_TYPE * Objmempool<OBJ_TYPE>::cache = NULL;

#endif /* OBJMEMPOOL_H_ */
