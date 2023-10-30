//
// Created by alexoxorn on 2021-12-01.
//

#include "../../../common.h"
#include <unordered_map>
#include <variant>
#include <numeric>
#include <algorithm>
#include <queue>
#include <ranges>

namespace aoc2022::day07 {
    struct directory;
    using directory_ptr = std::unique_ptr<directory>;
    using entity = std::variant<directory_ptr, long>;

    template <class... Ts>
    struct overload : Ts... {
        using Ts::operator()...;
    };
    template <class... Ts>
    overload(Ts...) -> overload<Ts...>;

    struct directory : std::unordered_map<std::string, entity> {
        using std::unordered_map<std::string, entity>::unordered_map;
        struct directory_itr {
            using value_type = const directory;
            using reference_type = const directory&;
            using pointer_type = const directory*;
            using iterator_tag = std::forward_iterator_tag;
            using difference_type = long;

            std::queue<const directory*> dir_queue;
            directory_itr() = default;
            directory_itr(const directory& start) : dir_queue{{&start}} {}

            reference_type operator*() const { return *dir_queue.front(); }
            pointer_type operator->() const { return dir_queue.front(); }
            directory_itr& operator++() {
                for (const auto& file : **this | stdv::values | stdv::filter([](const auto& file) {
                         return std::holds_alternative<directory_ptr>(file);
                     })) {
                    dir_queue.push(std::get<directory_ptr>(file).get());
                }
                dir_queue.pop();
                return *this;
            }
            directory_itr operator++(int) {
                auto res{*this};
                ++*this;
                return res;
            }
            bool operator==(const directory_itr& other) const = default;
        };

        directory* parent = nullptr;
        mutable long filesize = 0;

        explicit directory(directory* parent) : parent{parent} {};

        void add_directory(const std::string& name) { this->try_emplace(name, std::make_unique<directory>(this)); };
        void add_file(const std::string& name, long size) { this->try_emplace(name, size); }
        directory& get_directory(const std::string& s) { return *std::get<directory_ptr>(this->at(s)); }

        long calculate_size() {
            if (filesize > 0) {
                return filesize;
            }
            auto x = *this | stdv::values | stdv::transform([](const auto& x) {
                return std::visit(overload{[](const directory_ptr& dir) { return dir->calculate_size(); },
                                           [](const long l) {
                                               return l;
                                           }},
                                  x);
            });
            return (this->filesize = std::accumulate(x.begin(), x.end(), 0l));
        }

        directory_itr dir_begin() const { return {*this}; }
        directory_itr dir_end() const { return {}; }
    };

    directory& get_root(const char* filename) {
        static std::optional<directory> root;
        if (root) {
            return *root;
        }
        root.emplace();
        directory* current = &*root;
        auto commands = get_stream<ox::line>(filename);
        for (const std::string& command : commands) {
            if (command == "$ cd /") {
                current = &*root;
                continue;
            }
            if (command == "$ cd ..") {
                current = current->parent;
                continue;
            }
            if (command.starts_with("$ cd ")) {
                current->add_directory(command.substr(5, 100));
                current = &(current->get_directory(command.substr(5, 100)));
                continue;
            }
            if (command.starts_with("$ ls")) {
                continue;
            }
            if (command.starts_with("dir ")) {
                current->add_directory(command.substr(4, 100));
                continue;
            }
            char* filename;
            long size = strtol(command.c_str(), &filename, 10);
            current->add_file(filename + 1, size);
        }

        root->calculate_size();
        return *root;
    }

    std::vector<long> get_directory_sizes(const char* filename) {
        const directory& root = get_root(filename);
        auto x = stdr::subrange(root.dir_begin(), root.dir_end())
               | stdv::transform(&directory::filesize);
        return {x.begin(), x.end()};
    }

    answertype puzzle1(const char* filename) {
        std::vector<long> dirs = get_directory_sizes(filename);
        auto big_dir = dirs | stdv::filter([](const auto& dir) { return dir <= 100000; });
        auto total_size = std::accumulate(big_dir.begin(), big_dir.end(), 0l);
        myprintf("The total size of folder <= 100000 is %ld\n", total_size);
        return total_size;
    }

    answertype puzzle2(const char* filename) {
        const directory& root = get_root(filename);
        long needed_space = root.filesize - (70'000'000 - 30'000'000);
        std::vector<long> dirs = get_directory_sizes(filename);
        stdr::sort(dirs);
        auto todelete = stdr::find_if(dirs, [&](long l) { return l >= needed_space; });
        myprintf("Directory to delete has size of %ld\n", *todelete);
        return *todelete;
    }
} // namespace aoc2022::day07