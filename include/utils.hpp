#pragma once

inline bool is_continuation(char c) {
    return (c & 0xC0) == 0x80;
}