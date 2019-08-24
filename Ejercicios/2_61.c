#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

int main(int argc, char const *argv[]) {
        /* Falopa (? */
        uint32_t x = 0;
        uint32_t all_ones = 4294967295;

        bool is_any_bit_1 = (x & all_ones) > 0;
        bool is_any_bit_0 = (x & all_ones) < all_ones;

        /* Less significative byte is the first one in little endian */
        /* so I shifted left 24 bits, to get the less significative ones */
        /* as the most significative ones followed by zeroes */
        bool is_any_less_sig_byte_bit_1 = ((x << 24) & all_ones) > 0;
        bool is_any_less_sig_byte_bit_0 = ((x << 24) & all_ones) < all_ones;

        printf("Is there any 1 in x : %d \n", is_any_bit_1);
        printf("Is there any 0 in x : %d \n", is_any_bit_0);
        printf("Is there any 1 in the less significative byte in x : %d \n", is_any_less_sig_byte_bit_1);
        printf("Is there any 0 in the less significative byte in x : %d \n", is_any_less_sig_byte_bit_0);


        return 0;
}
