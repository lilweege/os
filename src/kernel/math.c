#include "math.h"

f32 sqrt(f32 x) {
    asm("fsqrt" : "+t"(x));
    return x;
}

f32 tan(f32 x) {
    asm("fptan;"
        "fstp %0"
        : "+t"(x));
    return x;
}

f32 sin(f32 x) {
    asm("fsin" : "+t"(x));
    return x;
}

f32 cos(f32 x) {
    asm("fcos" : "+t"(x));
    return x;
}
