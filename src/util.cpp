#include "util.h"

bool ut::validUNC(char c) {
    return ('a' <= c && c <= 'z') || ('A' <= c && c <='Z') || c == '_' || ('0' <= c && c <= '9');
}

bool ut::notValidUNC(char c) {
    return !validUNC(c);
}


