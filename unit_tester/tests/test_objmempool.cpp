/*
 * test_objmempool.cpp
 *
 */

#include "CppUTest/TestHarness.h"

#include "objmempool.h"

class Test_object : private Objmempool<Test_object>
{
public:
    Test_object() : id(0) {};

    uint64_t get_id() {return this->id;}
    void set_it(uint64_t id_) {id = id_;}

    // Mempool interface
    using Objmempool<Test_object>::mempool_create;
    using Objmempool<Test_object>::mempool_destroy;
    using Objmempool<Test_object>::get_mempool_size;
    using Objmempool<Test_object>::operator new;
    using Objmempool<Test_object>::operator delete;

private:
    uint64_t id;
};


TEST_GROUP(mempool)
{
    void setup()
    {

    }

    void teardown()
    {

    }
};

TEST(mempool, create_and_destroy_pool__check_pool_size)
{
    const size_t pool_size = 32;
    Test_object::mempool_create(pool_size);
    LONGS_EQUAL(pool_size-1, Test_object::get_mempool_size());
    Test_object::mempool_destroy();
    LONGS_EQUAL(0, Test_object::get_mempool_size());
}

TEST(mempool, new_delete_1_object__check_pool_size_decreased_by_1)
{
    const size_t pool_size = 32;

    Test_object::mempool_create(pool_size);

    Test_object * obj = new Test_object;
    LONGS_EQUAL(pool_size - 2, Test_object::get_mempool_size());

    delete obj;
    LONGS_EQUAL(pool_size - 1, Test_object::get_mempool_size());

    Test_object::mempool_destroy();
}
