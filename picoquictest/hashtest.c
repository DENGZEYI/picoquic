#include "../picoquic/picohash.h"

struct hashtestkey
{
    uint64_t x;
};

static uint64_t hashtest_hash(void * v)
{
    struct hashtestkey * k = (struct hashtestkey *) v;
    uint64_t hash = (k->x + 0xDEADBEEFull);
    return hash;
}

static int hashtest_compare(void * v1, void *v2)
{
    struct hashtestkey * k1 = (struct hashtestkey *) v1;
    struct hashtestkey * k2 = (struct hashtestkey *) v2;

    return (k1->x == k2->x) ? 0 : -1;
}

static picohash_item * hashtest_item(uint64_t x)
{
    struct hashtestkey * p = (struct hashtestkey *) malloc(sizeof(struct hashtestkey));

    if (p != NULL)
    {
        p->x = x;
    }

    return p;
}

int picohash_test()
{
    /* Create a hash table */
    int ret = 0;
    const int bin_size = 32;
    picohash_table * t = picohash_create(32, hashtest_hash, hashtest_compare);

    if (t == NULL)
    {
        ret = -1;
    }
    else
    {
        struct hashtestkey hk;

        /* Enter a bunch of values, all different */
        for (uint64_t i = 1; ret == 0 && i < 10; i += 2)
        {
            ret = picohash_insert(t, hashtest_item(i));
        }

        /* Test whether each value can be retrieved */
        for (uint64_t i = 1; ret == 0 && i < 10; i += 2)
        {
            hk.x = i;
            picohash_item * pi = picohash_retrieve(t, &hk);

            ret = (pi != NULL)?0:-1;
        }

        /* Create a bunch of collisions */
        for (uint64_t k = 1; ret == 0 && k < 6; k += 4)
        {
            for (uint64_t j = 1; ret == 0 && j <= k; j++)
            {
                ret = picohash_insert(t, hashtest_item(k + 32 * j));
            }
        }

        /* Check that the collisions can be retrieved */
        for (uint64_t k = 1; ret == 0 && k < 6; k += 4)
        {
            for (uint64_t j = 1; ret == 0 && j <= k; j++)
            {
                hk.x = k + 32 * j;
                picohash_item * pi = picohash_retrieve(t, &hk);

                ret = (pi != NULL) ? 0 : -1;
            }
        }

        /* Test whether different values cannot be retrieved */
        for (uint64_t i = 0; ret == 0 && i <= 10; i += 2)
        {
            hk.x = i;
            picohash_item * pi = picohash_retrieve(t, &hk);

            ret = (pi == NULL)?0:-1;
        }

        /* Delete first, last and middle */
        for (uint64_t i = 1; ret == 0 && i < 10; i += 4)
        {
            hk.x = i;
            picohash_item * pi = picohash_retrieve(t, &hk);

            if (pi == NULL)
            {
                ret = -1;
            }
            else
            {
                picohash_item_delete(t, pi, 1);
            }
        }

        /* Check that the deleted are gone */

        for (uint64_t i = 1; ret == 0 && i < 10; i += 4)
        {
            hk.x = i;
            picohash_item * pi = picohash_retrieve(t, &hk);

            if (pi != NULL)
            {
                ret = -1;
            }
        }

        /* Delete the table */
        picohash_delete(t, 1);
    }

    return ret;
}