#pragma once

#include <cxxopts.hpp>

class CLArgs {
public:
    cxxopts::ParseResult* args;

    CLArgs(cxxopts::ParseResult* args) {
        this->args = args;
    }

    bool getBool(const std::string& option) {
        return (*args)[option].as<bool>();
    }

    int getInt(const std::string& option) {
        return (*args)[option].as<int>();
    }

    float getFloat(const std::string& option) {
        return (*args)[option].as<float>();
    }

    std::string getString(const std::string& option) {
        return (*args)[option].as<std::string>();
    }

};