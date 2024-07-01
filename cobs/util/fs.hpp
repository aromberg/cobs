/*******************************************************************************
 * cobs/util/fs.hpp
 *
 * Copyright (c) 2018 Florian Gauger
 *
 * All rights reserved. Published under the MIT License in the LICENSE file.
 ******************************************************************************/

#ifndef COBS_UTIL_FS_HEADER
#define COBS_UTIL_FS_HEADER

#include <filesystem>
namespace cobs {
  namespace fs = std::filesystem;
  using std::error_code;
} // namespace cobs

#endif // !COBS_UTIL_FS_HEADER

/******************************************************************************/