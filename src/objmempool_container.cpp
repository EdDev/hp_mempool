/*
 * objmempool_container.cpp
 *
 */

#include "objmempool_container.h"

Objmempool_container * Objmempool_container::selfie = NULL;

void Objmempool_container::add(uint8_t * obj_memory_head,
                               size_t  obj_size,
                               size_t  obj_count,
                               func_show_cmd show_cmd)
{
    if(selfie == NULL)
        selfie = new Objmempool_container();

    Mempool_record mp_rec = {obj_memory_head, obj_size, obj_count, show_cmd};
    selfie->mprecord.push_back(mp_rec);
}

std::size_t Objmempool_container::size()
{
    if(selfie == NULL)
        return 0;

    return selfie->mprecord.size();
}

void Objmempool_container::clear()
{
    if(selfie)
    {
        delete selfie;
        selfie = NULL;
    }
}

int Objmempool_container::show_cmd(int argc, const char **argv, char *buf, std::size_t buf_size)
{
    mp_records & mpr = selfie->mprecord;
    char * const buf_base = buf;

    for(mp_records::iterator record = mpr.begin(); record != mpr.end(); ++record )
    {
        int ch_num = record->show_cmd(argc, argv, buf, buf_size);

        buf += ch_num;
        buf_size -= ch_num;
    }

    return (buf - buf_base);
}
