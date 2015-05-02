/*
 * test_objmempool.cpp
 *
 */

#include "CppUTest/TestHarness.h"

#include "objmempool.h"
#include "objmempool_container.h"

class Test_object : public Objmempool<Test_object>
{
public:
    Test_object() : id(0) {};

    uint64_t get_id() {return this->id;}
    void set_id(uint64_t id_) {id = id_;}

private:
    uint64_t id;
};

class Test_object_ctor : public Objmempool<Test_object_ctor>
{
public:
    Test_object_ctor(int i) : id(i) {};

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
        Objmempool_container::clear();
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
    LONGS_EQUAL(pool_size - 2, Test_object::get_mempool_free_obj_count());

    delete obj;
    LONGS_EQUAL(pool_size - 1, Test_object::get_mempool_free_obj_count());

    Test_object::mempool_destroy();
}

TEST(mempool_basic, create_and_destroy_pool_with_ctor__check_pool_size)
{
    const size_t pool_size = 32;
    Test_object_ctor::mempool_create(pool_size);
    LONGS_EQUAL(pool_size-1, Test_object_ctor::get_mempool_size());
    Test_object_ctor::mempool_destroy();
    LONGS_EQUAL(0, Test_object_ctor::get_mempool_size());
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

    	Objmempool_container::clear();
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

TEST(mempool, use_nothrow_tag__object_provided_from_pool)
{
    LONGS_EQUAL(POOL_SIZE - 1, Test_object::get_mempool_free_obj_count());

    Test_object * obj = new (std::nothrow) Test_object;

    LONGS_EQUAL(POOL_SIZE - 2, Test_object::get_mempool_free_obj_count());

    delete obj;
}

TEST(mempool, use_objmempool_container)
{
    const size_t pool_size = 32;
    Test_object_ctor::mempool_create(pool_size);

    const size_t expected_num_of_mempools_created = 2;
    LONGS_EQUAL(expected_num_of_mempools_created, Objmempool_container::size());

    Test_object_ctor::mempool_destroy();
}

TEST(mempool, show_cmd_for_all_objmempools)
{
    const size_t pool_size = 32;
    Test_object_ctor::mempool_create(pool_size);

    const size_t buf_size = 1024;
    char buf[buf_size];
    Objmempool_container::show_cmd(0, NULL, buf, buf_size);

    Test_object_ctor::mempool_destroy();
}

//TEST(mempool, new_delete_object_array__compilation_error)
//{
//    Test_object * obj = new Test_object[2];
//    delete [] obj;
//}
