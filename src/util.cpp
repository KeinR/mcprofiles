#include "util.h"

std::vector<std::string> ut::tokenize(const std::string &in) {
    std::vector<std::string> result;
    result.emplace_back();
    bool escape = false;
    bool lastSpace = false;
    for (char c : in) {
        lastSpace = false;
        if (escape) {
            if (c == '"') {
                escape = false;
            } else {
                result.back().push_back(c);
            }
        } else {
            switch (c) {
                case ' ':
                    if (!lastSpace) {
                        result.emplace_back();
                    }
                    lastSpace = true;
                    break;
                case '"':
                    escape = true;
                    break;
                default:
                    result.back().push_back(c);
            }
        }
    }
    
    return result;
}

