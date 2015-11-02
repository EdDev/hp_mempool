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

enum {POOL_SIZE = 256};
TEST_GROUP(mempool)
{
    void setup()
    {
    	Test_object::mempool_create(POOL_SIZE);
    	Test_object::mempool_cache_create();
    }

    void teardown()
    {
        Test_object::mempool_cache_destroy();
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
    // Creating a pool of N objects, gives N-1 usable objects.
    // The "missing" object is reserved by the free-list to differentiate between
    // an empty list and a full list.
    LONGS_EQUAL(POOL_SIZE - 1, Test_object::get_mempool_free_obj_count());

    Test_object * obj = new (std::nothrow) Test_object;

    // When cache is enabled, we will have in the pool N - CACHE-OBJ-COUNT.
    LONGS_EQUAL(POOL_SIZE - Test_object::CACHE_SIZE_DEFAULT - 1, Test_object::get_mempool_free_obj_count());

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

#include <pthread.h>
#include <unistd.h>

static void * thread_alloc(void * arg)
{
    static __thread Test_object * membuff = NULL;

    Test_object::mempool_cache_create();

    int * sig_post_alloc = static_cast<int *>(arg);
    if(membuff == NULL)
        membuff = new Test_object();

    while( ! *sig_post_alloc )
        sleep(1);

    if(membuff != NULL)
        delete membuff;

    membuff = NULL;

    Test_object::mempool_cache_destroy();
    return NULL;
}

static void * thread_alloc_all_pool(void * arg)
{
    //const size_t mempool_size = Test_object::get_mempool_size();
    static __thread Test_object * membuff[POOL_SIZE-1];

    Test_object::mempool_cache_create();

    size_t mp_size = Test_object::get_mempool_size() - Test_object::CACHE_SIZE_DEFAULT;
    while(mp_size)
    {
        --mp_size;
        if(membuff[mp_size] == NULL)
            membuff[mp_size] = new Test_object();
    }

    int * sig_post_alloc = static_cast<int *>(arg);
    while( ! *sig_post_alloc )
        sleep(1);

    mp_size = Test_object::get_mempool_size() - Test_object::CACHE_SIZE_DEFAULT;
    while(mp_size)
    {
        --mp_size;
        if(membuff[mp_size] != NULL)
            delete membuff[mp_size];

        membuff[mp_size] = NULL;
    }

    Test_object::mempool_cache_destroy();
    return NULL;
}

TEST(mempool, use_cache_from_two_threads__chace_bulk_is_not_returned)
{
    int sig = 0;
    pthread_t alloc_thread;

    LONGS_EQUAL(POOL_SIZE - 1, Test_object::get_mempool_free_obj_count());

    pthread_create(&alloc_thread, NULL, thread_alloc, &sig);
    sleep(2);

    // When cache is enabled, we will have in the pool N - CACHE-OBJ-COUNT.
    LONGS_EQUAL(POOL_SIZE - Test_object::CACHE_SIZE_DEFAULT - 1, Test_object::get_mempool_free_obj_count());

    sig = 1; // Continue to free.
    sleep(2);

    // The cache bulk is not returned to the pool: It is retuned only when it reaches the threshold.
    LONGS_EQUAL(POOL_SIZE - Test_object::CACHE_SIZE_DEFAULT - 1, Test_object::get_mempool_free_obj_count());

    pthread_join(alloc_thread, NULL);
}

TEST(mempool, use_cache_from_two_threads_allocate_all_return_all__consume_pool)
{
    int sig = 0;
    pthread_t alloc_thread;

    pthread_create(&alloc_thread, NULL, thread_alloc_all_pool, &sig);
    sleep(2);

    // When cache is enabled, we will have in the pool N - CACHE-OBJ-COUNT.
    LONGS_EQUAL(Test_object::CACHE_SIZE_DEFAULT - 1, Test_object::get_mempool_free_obj_count());

    sig = 1; // Continue to free.
    sleep(2);

    // The cache bulk is not returned to the pool: It is retuned only when it reaches the threshold.
    LONGS_EQUAL(POOL_SIZE - Test_object::CACHE_SIZE_DEFAULT - 1, Test_object::get_mempool_free_obj_count());

    pthread_join(alloc_thread, NULL);
}

//TEST(mempool, new_delete_object_array__compilation_error)
//{
//    Test_object * obj = new Test_object[2];
//    delete [] obj;
//}
