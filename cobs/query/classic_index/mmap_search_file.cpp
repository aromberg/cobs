/*******************************************************************************
 * cobs/query/classic_index/mmap_search_file.cpp
 *
 * Copyright (c) 2018 Florian Gauger
 *
 * All rights reserved. Published under the MIT License in the LICENSE file.
 ******************************************************************************/

#include <cobs/query/classic_index/mmap_search_file.hpp>
#include <cobs/util/file.hpp>
#include <cobs/util/fs.hpp>
#include <cobs/util/query.hpp>
#include <cstring>

namespace cobs {

ClassicIndexMMapSearchFile::ClassicIndexMMapSearchFile(const fs::path& path)
    : ClassicIndexSearchFile(path) {
    handle_ = initialize_mmap(path);
    data_ = handle_.data + stream_pos_.curr_pos;
}

ClassicIndexMMapSearchFile::ClassicIndexMMapSearchFile(std::ifstream &ifs, int64_t index_file_size)
    : ClassicIndexSearchFile(ifs, index_file_size) {
    handle_ = initialize_stream(ifs, stream_pos_.size());
    data_ = handle_.data;
}

ClassicIndexMMapSearchFile::~ClassicIndexMMapSearchFile() {
    destroy_mmap(handle_);
}

void ClassicIndexMMapSearchFile::read_from_disk(
    const std::vector<uint64_t>& hashes, uint8_t* rows,
    uint64_t begin, uint64_t size, uint64_t buffer_size)
{
    die_unless(begin + size <= header_.row_size());
    for (uint64_t i = 0; i < hashes.size(); i++) {
        auto data_8 =
            data_ + begin
            + (hashes[i] % header_.signature_size_) * header_.row_size();
        auto rows_8 = rows + i * buffer_size;
        // std::memcpy(rows_8, data_8, size);
        std::copy(data_8, data_8 + size, rows_8);
    }
}

} // namespace cobs

/******************************************************************************/
