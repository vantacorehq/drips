#include "drips.h"

#include <stdio.h>
#include <stdlib.h>

static int checks;

static void expect(int cond, const char *msg) {
    checks++;
    if (!cond) {
        fprintf(stderr, "fail: %s\n", msg);
        exit(1);
    }
}

int main(void) {
    drip bucket;
    const uint64_t t0 = 1000000000ull;

    drip_init(&bucket, 10, 20, t0);
    expect(drip_tokens(&bucket) == 20, "starts full");
    expect(drip_take(&bucket, t0, 20) == 0, "burst can be spent at once");
    expect(drip_tokens(&bucket) == 0, "empty after the burst");
    expect(drip_take(&bucket, t0, 1) == -1, "empty bucket refuses");

    expect(drip_take(&bucket, t0 + 500000000ull, 0) == 0, "zero take still refills");
    expect(drip_tokens(&bucket) == 5, "half a second at 10/s is 5");
    expect(drip_take(&bucket, t0 + 500000000ull, 5) == 0, "those 5 can be taken");
    expect(drip_take(&bucket, t0 + 500000000ull, 1) == -1, "nothing left in that instant");

    expect(drip_take(&bucket, t0 + 1500000000ull, 0) == 0, "refill across one more second");
    expect(drip_tokens(&bucket) == 10, "one second adds the rate");

    drip_init(&bucket, 10, 4, t0);
    expect(drip_take(&bucket, t0, 4) == 0, "spend a small burst");
    expect(drip_take(&bucket, t0 + 10ull * 1000000000ull, 0) == 0, "long idle refills");
    expect(drip_tokens(&bucket) == 4, "refill stops at the burst");
    expect(drip_take(&bucket, t0 + 10ull * 1000000000ull, 5) == -1, "cannot take more than the burst");

    drip_init(&bucket, 10, 4, t0);
    expect(drip_take(&bucket, t0, 1) == 0, "spend one");
    expect(drip_take(&bucket, t0 - 1, 0) == 0, "clock backwards is ignored");
    expect(drip_tokens(&bucket) == 3, "backwards clock did not mint tokens");

    drip_init(&bucket, 1000000000ull, 2, t0);
    expect(drip_take(&bucket, t0, 2) == 0, "drain a fast bucket");
    expect(drip_take(&bucket, t0 + 1, 0) == 0, "one nanosecond of a 1e9/s rate");
    expect(drip_tokens(&bucket) == 1, "one nanosecond mints one token");

    printf("drips: %d checks passed\n", checks);
    return 0;
}
