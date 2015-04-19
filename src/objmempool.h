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

#include <list>

template <typename OBJ_TYPE>
class Objmempool
{
    typedef std::list<OBJ_TYPE *> Free_list;

protected:
    Objmempool() {};
    ~Objmempool() {};

public:
    static void * operator new (size_t size)
    {
        UNUSED(size);
        void * obj = free_list->front();
        free_list->pop_front();
        return obj;
    }

    static void operator delete (void * m)
    {
        free_list->push_front(static_cast<OBJ_TYPE *>(m));
    }

    static void mempool_create(size_t object_count)
    {
        if(NULL == free_list)
        {
            free_list = ::new Free_list();
            obj_count = object_count;

            for (int i=0; i < object_count; ++i)
                free_list->push_back(::new OBJ_TYPE());
        }
    }

    static void mempool_destroy()
    {
        for (int i=0; i < obj_count; ++i)
        {
            OBJ_TYPE * obj = free_list->front();
            ::delete obj;
            free_list->pop_front();
        }

        ::delete free_list;
        obj_count = 0;
        free_list = NULL;
    }

    static size_t get_mempool_size()
    {
        if(NULL != free_list)
            return free_list->size();
        else
            return 0;
    }

private:
    static Free_list * free_list;
    static size_t obj_count;
};

template <typename OBJ_TYPE>
size_t Objmempool<OBJ_TYPE>::obj_count = 0;

template <typename OBJ_TYPE>
typename Objmempool<OBJ_TYPE>::Free_list * Objmempool<OBJ_TYPE>::free_list = NULL;

#endif /* OBJMEMPOOL_H_ */
