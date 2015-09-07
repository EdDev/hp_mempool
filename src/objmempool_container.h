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
