#include "util.h"

/**
 * memset - Fill a block of memory with a specified value
 * @dest: Pointer to the destination memory block to fill
 * @val: Value to set each byte to (typically 0 for clearing memory)
 * @count: Number of bytes to set
 * 
 * This function sets 'count' bytes of memory starting at 'dest' to the
 * specified value 'val'. Commonly used for initializing arrays, clearing
 * structures, or zeroing memory blocks.
 */
void memset(void *dest, char val, uint32_t count) {
    // Cast void pointer to char pointer for byte-by-byte access
    char *temp = (char*) dest;
    
    // Loop through each byte in the memory block
    for (; count != 0; count--) {
        // Set current byte to the specified value and advance pointer
        *temp++ = val;  // Equivalent to: *temp = val; temp++;
    }
}