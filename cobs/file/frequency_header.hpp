/*******************************************************************************
 * cobs/file/frequency_header.hpp
 *
 * Copyright (c) 2018 Florian Gauger
 *
 * All rights reserved. Published under the MIT License in the LICENSE file.
 ******************************************************************************/

#ifndef COBS_FILE_FREQUENCY_HEADER_HEADER
#define COBS_FILE_FREQUENCY_HEADER_HEADER

#include <cobs/file/header.hpp>

namespace cobs {

class FrequencyHeader : public Header<FrequencyHeader>
{
protected:
    void serialize(std::ofstream& ost) const override;
    void deserialize(std::ifstream& ifs) override;

public:
    static const std::string magic_word;
    static const std::string file_extension;
};

} // namespace cobs

#endif // !COBS_FILE_FREQUENCY_HEADER_HEADER

/******************************************************************************/
