#include "msbf_header.hpp"

namespace genome::file {
    const std::string msbf_header::magic_word = "MSBF";
    const std::string msbf_header::file_extension = ".g_mbf";


    size_t msbf_header::padding_size(long stream_position) const {
        return (m_page_size - ((stream_position + msbf_header::magic_word.size()) % m_page_size)) % m_page_size;
    }

    void msbf_header::serialize(std::ofstream& ofs) const {
        genome::serialize(ofs, (uint32_t) m_parameters.size());
        ofs.flush();
        for (const auto& parameter: m_parameters) {
            genome::serialize(ofs, std::get<0>(parameter), std::get<1>(parameter), std::get<2>(parameter));
        }
        for (const auto& file_name: m_file_names) {
            ofs << file_name << std::endl;
        }

        ofs.flush();
        std::vector<char> padding(padding_size(ofs.tellp()));
        ofs.write(padding.data(), padding.size());
        ofs.flush();
    }

    void msbf_header::deserialize(std::ifstream& ifs) {
        uint32_t parameters_size;
        genome::deserialize(ifs, parameters_size);
        m_parameters.resize(parameters_size);
        for (auto& parameter: m_parameters) {
            genome::deserialize(ifs, std::get<0>(parameter), std::get<1>(parameter), std::get<2>(parameter));
        }
        for (auto& file_name: m_file_names) {
            std::getline(ifs, file_name);
        }

        ifs.ignore(padding_size(ifs.tellg()));
    }

    msbf_header::msbf_header(uint32_t page_size) : m_page_size(page_size) {}

    msbf_header::msbf_header(const std::vector<std::tuple<uint64_t, uint64_t, uint64_t>>& parameters, const std::vector<std::string>& file_names, uint32_t page_size)
            : m_parameters(parameters), m_file_names(file_names), m_page_size(page_size) {}

    const std::vector<std::tuple<uint64_t, uint64_t, uint64_t>>& msbf_header::parameters() const {
        return m_parameters;
    }

    const std::vector<std::string>& msbf_header::file_names() const {
        return m_file_names;
    }
}
