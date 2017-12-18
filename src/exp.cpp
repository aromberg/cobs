#include <CLI/CLI.hpp>
#include <zconf.h>
#include <cmath>
#include <ctime>
#ifdef NO_AIO
#include <isi/query/compact_index/mmap.hpp>
#include <map>

#else
#include <isi/query/compact_index/aio.hpp>
#endif

template<typename T>
void check_between(std::string name, T num, T min, T max, bool min_inclusive, bool max_inclusive) {
    if ((min_inclusive && num < min) || (!min_inclusive && num <= min) || (max_inclusive && num > max) || (!max_inclusive && num >= max)) {
        std::string interval = (min_inclusive ? "[" : "(") + std::to_string(min) + ", " + std::to_string(max) + (max_inclusive ? "]" : ")");
        throw std::invalid_argument(name + " (" + std::to_string(num) + ") must be in the interval " + interval);
    }
}

uint64_t get_unsigned_integer(const std::string& value, const std::string& name, uint64_t min = 0, uint64_t max = UINT64_MAX, bool min_inclusive = true, bool max_inclusive = true) {
    uint64_t ui;
    double d;
    try {
        d = std::stod(value);
        ui = std::stoull(value);
    } catch (...) {
        throw std::invalid_argument(name + " (" + value + ") is not an unsigned integer");
    }

    if (d < 0) {
        throw std::invalid_argument(name + " (" + value + ") is negative");
    } else if (std::floor(d) != d) {
        throw std::invalid_argument(name + " (" + value + ") is not an integer");
    }

    check_between<uint64_t>(name, ui, min, max, min_inclusive, max_inclusive);
    return ui;
}
std::experimental::filesystem::path get_path(const std::string& value, const std::string& name, bool needs_to_exist = false) {
    std::experimental::filesystem::path p(value);
    if(needs_to_exist && !std::experimental::filesystem::exists(p)) {
        throw std::invalid_argument("path " + name + " (" + p.string() + ") does not exist");
    }
    return p;
}

struct parameters {
    uint64_t num_kmers;
    uint64_t num_exps;
    uint64_t num_warmup_exps;
    std::experimental::filesystem::path in_file;

    CLI::Option* add_num_kmers(CLI::App* app) {
        return app->add_option("<num_kmers>", [&](std::vector<std::string> val) {
            this->num_kmers = get_unsigned_integer(val[0], "<num_kmers>", 1);
            return true;
        }, "number of kmers of each query");
    }

    CLI::Option* add_num_exps(CLI::App* app) {
        return app->add_option("<num_exps>", [&](std::vector<std::string> val) {
            this->num_exps = get_unsigned_integer(val[0], "<num_exps>", 1);
            return true;
        }, "number of experiments");
    }

    CLI::Option* add_num_warmup_exps(CLI::App* app) {
        return app->add_option("<num_warmup_exps>", [&](std::vector<std::string> val) {
            this->num_warmup_exps = get_unsigned_integer(val[0], "<num_warmup_exps>", 1);
            return true;
        }, "number of warmup experiments");
    }

    CLI::Option* add_in_file(CLI::App* app) {
        return app->add_option("<in_file>", [&](std::vector<std::string> val) {
            this->in_file = get_path(val[0], "<in_file>", true);
            return true;
        }, "path to the input file");
    }
};

void run(const std::experimental::filesystem::path p, size_t num_kmers, size_t num_iterations, size_t num_warmup_iterations) {
#ifdef NO_AIO
    isi::query::compact_index::mmap s(p);
#else
    isi::query::compact_index::aio s(p);
#endif

    std::vector<std::pair<uint16_t, std::string>> result;
    sync();
    for (size_t i = 0; i < num_warmup_iterations; i++) {
        std::string query = isi::random_sequence(num_kmers + 30, std::time(nullptr));
        s.search(query, 31, result);
    }
    s.get_timer().reset();

#ifdef FALSE_POSITIVE_DIST
    std::map<uint16_t, double> counts;
#endif

    for (size_t i = 0; i < num_iterations; i++) {
        std::string query = isi::random_sequence(num_kmers + 30, std::time(nullptr));
        s.search(query, 31, result);
#ifdef FALSE_POSITIVE_DIST
        for (const auto& r: result) {
            counts[r.first]++;
        }
#endif
    }

#ifdef QUERY
    std::string simd = "on";
    std::string openmp = "on";
    std::string aio = "on";

#ifdef NO_SIMD
    simd = "off";
#endif

#ifdef NO_OPENMP
    openmp = "off";
#endif

#ifdef NO_AIO
    aio = "off";
#endif

    isi::timer t = s.get_timer();
    std::cout << p.string() << "," << num_kmers << "," << num_iterations << "," << num_warmup_iterations << ",";
    std::cout << simd << "," << openmp << "," << aio << ",";
    std::cout << t.get("hashes") << "," << t.get("io") << "," << t.get("and rows") << "," << t.get("add rows")
              << "," << t.get("sort results") << std::endl;
#endif

#ifdef FALSE_POSITIVE_DIST
    for (const auto& c: counts) {
        std::cout << c.first << "," << (c.second / num_iterations / result.size()) << std::endl;
    }
#endif
}

int main(int argc, char **argv) {
    CLI::App app("Experiments", false);
    auto p = std::make_shared<parameters>();
    p->add_num_kmers(&app)->required();
    p->add_num_exps(&app)->required();
    p->add_num_warmup_exps(&app)->required();
    p->add_in_file(&app)->required();
    CLI11_PARSE(app, argc, argv);

    run(p->in_file, p->num_kmers, p->num_exps, p->num_warmup_exps);
}