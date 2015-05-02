/*
 * objmempool_container.h
 *
 */

#ifndef OBJMEMPOOL_CONTAINER_H_
#define OBJMEMPOOL_CONTAINER_H_

#include <stdint.h>
#include <vector>

class Objmempool_container
{
public:
    typedef int (*func_show_cmd)(int argc, const char **argv, char *buf, std::size_t buf_size);

    static void add(uint8_t * obj_memory_head,
                    size_t    obj_size,
                    size_t    obj_count,
                    func_show_cmd show_cmd);

    static std::size_t size();

    static int show_cmd(int argc, const char **argv, char *buf, std::size_t buf_size);

    static void clear();

private:
    Objmempool_container() {}
    static Objmempool_container * selfie;

    struct Mempool_record
    {
        uint8_t * obj_memory_head;    // Start of object pool memory.
        size_t    obj_size;
        size_t    obj_count;
        func_show_cmd show_cmd;

    };
    typedef std::vector<Mempool_record> mp_records;
    mp_records mprecord;
};

#endif /* OBJMEMPOOL_CONTAINER_H_ */
