#include <sys/time.h>

#define TI(tag)    \
    struct timeval time_start_##tag, time_end_##tag;\
    do{ \
    gettimeofday(&time_start_##tag, NULL);}while(0)

#define TO(tag, func)     \
    do{\
    gettimeofday(&time_end_##tag, NULL);\
    cout << func << " " << #tag":\t" << (time_end_##tag.tv_sec - time_start_##tag.tv_sec) * 1000.0 + (time_end_##tag.tv_usec - time_start_##tag.tv_usec) / 1000.0 << " ms" << endl;\
    }while(0)

