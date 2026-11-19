#ifndef DRIPS_H
#define DRIPS_H

#include <stdint.h>

/* Integer token bucket. The caller owns the clock.
   One token is one unit. Time is monotonic nanoseconds. */

typedef struct drip {
    uint64_t tokens;
    uint64_t rate;     /* tokens added per second */
    uint64_t burst;    /* maximum tokens held */
    uint64_t last_ns;
} drip;

/* Starts full: burst tokens are available at now_ns.
   rate_per_sec and burst must be greater than zero. */
void drip_init(drip *bucket, uint64_t rate_per_sec, uint64_t burst, uint64_t now_ns);

/* Refill up to now_ns, then take n tokens.
   Returns 0 when the tokens were taken, -1 when they were not.
   n == 0 only refills. A clock that moves backwards does not add tokens. */
int drip_take(drip *bucket, uint64_t now_ns, uint64_t n);

uint64_t drip_tokens(const drip *bucket);

#endif
