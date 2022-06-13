#include <array>
#include <vector>
#include <tuple>
#include <map>
#include <regex>
#include <iostream>
#include <string>
#include <fstream>

#include <cerrno>
#include <cstring>


const std::string PROG_NAME = "cat";


std::tuple<std::map<char, int>, std::vector<std::string>> parse_opts(int argc, char* argv[]) {
    std::regex parse_opts_regex("[a-z]");

    std::map<char, int> opts {};
    std::vector<std::string> filenames {};

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

void cat(std::string filename, std::map<char, int> opts) {
    char buf[BUFSIZ];
    std::ifstream fs(filename, std::ios_base::in);

    if (fs.is_open()) {
        for (;;) {
            fs.read(buf, BUFSIZ);
            auto gcount = fs.gcount();

            if (!fs && gcount <= 0) break;

            std::cout.write(buf, gcount);
        }
    } else {
        std::cerr << PROG_NAME << ": " << filename << ": " << std::strerror(errno) << std::endl;
    }
}


int main(int argc, char* argv[]) {
    auto opts_n_files = parse_opts(argc, argv);

    auto opts = std::get<0>(opts_n_files);
    auto filenames = std::get<1>(opts_n_files);

    for (int i = 1; i < filenames.size(); i++) {
        cat(filenames[i], opts);
    }

    return 0;
}