#include "drips.h"

enum { DRIP_NS = 1000000000ull };

void drip_init(drip *bucket, uint64_t rate_per_sec, uint64_t burst, uint64_t now_ns) {
    bucket->tokens = burst;
    bucket->rate = rate_per_sec;
    bucket->burst = burst;
    bucket->last_ns = now_ns;
}

static uint64_t add_capped(uint64_t have, uint64_t add, uint64_t cap) {
    if (add >= cap || have >= cap - add) {
        return cap;
    }
    return have + add;
}

static void refill(drip *bucket, uint64_t now_ns) {
    uint64_t elapsed;
    uint64_t whole;
    uint64_t extra;
    __uint128_t product;

    if (now_ns <= bucket->last_ns || bucket->rate == 0) {
        return;
    }
    elapsed = now_ns - bucket->last_ns;
    bucket->last_ns = now_ns;

    whole = elapsed / DRIP_NS;
    product = (__uint128_t)whole * bucket->rate;
    if (product >= bucket->burst) {
        bucket->tokens = bucket->burst;
        return;
    }
    bucket->tokens = add_capped(bucket->tokens, (uint64_t)product, bucket->burst);

    extra = (uint64_t)(((__uint128_t)(elapsed % DRIP_NS) * bucket->rate) / DRIP_NS);
    bucket->tokens = add_capped(bucket->tokens, extra, bucket->burst);
}

int drip_take(drip *bucket, uint64_t now_ns, uint64_t n) {
    refill(bucket, now_ns);
    if (n > bucket->tokens) {
        return -1;
    }
    bucket->tokens -= n;
    return 0;
}

uint64_t drip_tokens(const drip *bucket) {
    return bucket->tokens;
}
