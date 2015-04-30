/*
 * test_objmempool.cpp
 *
 */

#include "CppUTest/TestHarness.h"

#include "objmempool.h"

class Test_object : public Objmempool<Test_object>
{
public:
    Test_object() : id(0) {};

    uint64_t get_id() {return this->id;}
    void set_id(uint64_t id_) {id = id_;}

private:
    uint64_t id;
};


TEST_GROUP(mempool_basic)
{
    void setup()
    {

    }

    void teardown()
    {

    }
};

TEST(mempool_basic, create_and_destroy_pool__check_pool_size)
{
    const size_t pool_size = 32;
    Test_object::mempool_create(pool_size);
    LONGS_EQUAL(pool_size-1, Test_object::get_mempool_size());
    Test_object::mempool_destroy();
    LONGS_EQUAL(0, Test_object::get_mempool_size());
}

TEST(mempool_basic, new_delete_1_object__check_pool_size_decreased_by_1)
{
    const size_t pool_size = 32;

    Test_object::mempool_create(pool_size);

    Test_object * obj = new Test_object;
    LONGS_EQUAL(pool_size - 2, Test_object::get_mempool_size());

    delete obj;
    LONGS_EQUAL(pool_size - 1, Test_object::get_mempool_size());

    Test_object::mempool_destroy();
}



TEST_GROUP(mempool)
{
	enum {POOL_SIZE = 32};

    void setup()
    {
    	Test_object::mempool_create(POOL_SIZE);
    }

    void teardown()
    {
    	Test_object::mempool_destroy();
    }
};

TEST(mempool, access_test_object_data__check_object_data_integrity)
{
	const int data = 12345;
	Test_object * obj = new Test_object;

	obj->set_id(data);
	LONGS_EQUAL(data, obj->get_id());

	delete obj;
}

//TEST(mempool, new_delete_object_array__compilation_error)
//{
//    Test_object * obj = new Test_object[2];
//    delete [] obj;
//}
