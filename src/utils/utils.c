#include "utils.h"

inline int32_t max_int(int x, int y) {
    return x > y ? x : y;
}

inline int32_t min_int(int x, int y) {
    return x < y ? x : y;
}

inline int32_t clamp_int(int val, int lo, int hi) {
    return max_int(lo, min_int(hi, val));
}