#include <array>
#include <vector>
#include <map>
#include <regex>
#include <iostream>
#include <fstream>

#include <unistd.h>

#include <chrono>
#include <thread>

const std::string PROG_NAME = "cat";


std::tuple <std::map<char, int>, std::vector<std::string>> parse_opts(int argc, char *argv[]) {
    std::regex parse_opts_regex("[a-z]");

    std::map<char, int> opts{};
    std::vector <std::string> filenames{};

    for (auto i = 0; i < argc; i++) {
        std::string arg(argv[i]);

        if (arg[0] == '-') { // arg is opt
            auto opts_begin = std::sregex_iterator(arg.begin(), arg.end(), parse_opts_regex);
            auto opts_end = std::sregex_iterator();

            for (std::sregex_iterator i = opts_begin; i != opts_end; i++) {
                std::smatch match = *i;
                char opt = match.str()[0];
                opts[opt] += 1;
            }
        } else { // arg is filename
            filenames.push_back(arg);
        }
    }

    return std::make_pair(opts, filenames);
}

void cat(std::istream& is, const std::map<char, int>& opts) {
    char buf[BUFSIZ];

    for (;;) {
        is.read(buf, BUFSIZ);
        auto gcount = is.gcount();

        if (gcount > 0) std::cout.write(buf, gcount);
        if (is.eof()) break;
    }
}


int main(int argc, char *argv[]) {
    auto opts_n_files = parse_opts(argc, argv);

    auto opts = std::get<0>(opts_n_files);
    auto filenames = std::get<1>(opts_n_files);

    auto filecount = filenames.size();

    if (filecount > 1) {
        for (int i = 1; i < filenames.size(); i++) {
            std::ifstream fs(filenames[i], std::ios_base::in);
            if (fs.is_open()) {
                cat(fs, opts);
            } else {
                std::cerr << PROG_NAME << ": " << filenames[i] << ": " << std::strerror(errno) << std::endl;
            }
        }
    } else {
        cat(std::cin, opts);
    }

    return 0;
}