# buddy_allocator_bitmap

A buddy memory allocator backed by a bitmap tree, written in C. Developed as an extension of a reference implementation from the Operating Systems course at Sapienza University of Rome.

## Building

A fresh clone requires no manual setup. All output directories are created automatically by the build system.

```sh
make buddy        # build and run the demo
make test         # build and run all tests
make buddyVG      # run under Valgrind
make buddyGProf   # profile with gprof
make clean        # remove all build artifacts
```

To enable debug output (prints allocator state to stdout on every malloc/free):

```sh
make buddy CCOPTS="-Wall -Wextra -g -std=gnu99 -DDEBUG=1"
```

## How it works

The allocator manages a flat byte buffer using a **bitmap-backed binary tree** for bookkeeping. Memory is partitioned into power-of-two-sized regions ("buddies") at successive levels of a complete binary tree. Each level `l` occupies indices `[2^l, 2^(l+1) - 1]` in the bitmap.

On `BuddyAllocator_malloc(size)`:
1. The smallest level whose block size can fit `size` is calculated.
2. The first free node at that level is found and its index is written into the first 4 bytes of the returned memory region.
3. All parent nodes are marked allocated. All child nodes are marked allocated.

On `BuddyAllocator_free(ptr)`:
1. The tree index is read from the 4 bytes preceding `ptr`.
2. The node is marked free. All children are marked free.
3. Parent nodes are freed only if the buddy of each node is also free (coalescing).

The buffer layout is:

```
[ BuddyAllocator | BitMap_tree | BitMap | bitmap bytes | ... user memory ... ]
```

Each allocated user region is prefixed with 4 bytes storing its tree index, enabling O(log n) free without an external lookup table.

## Error handling

`BuddyAllocator_releaseBuddy` returns one of five result codes:

| Code | Meaning |
|---|---|
| `BA_Success` | Operation succeeded |
| `BA_NotEnoughMemory` | No free block large enough to satisfy the request |
| `BA_UnalignedFree` | Pointer is not aligned to a bucket boundary |
| `BA_OutOfRange` | Pointer is outside the allocator's memory region |
| `BA_DoubleFree` | Pointer points to a region already marked free |

`BuddyAllocator_malloc` aborts with `assert` on `BA_NotEnoughMemory`. `BuddyAllocator_free` aborts on a `NULL` pointer. Out-of-range, unaligned, and double-free conditions in `BuddyAllocator_free` currently terminate via the return value of `releaseBuddy`; callers can check the result directly via `BuddyAllocator_releaseBuddy` for recoverable error handling.

## Possible extensions

- **Recoverable error handling in free**: expose `BuddyAllocator_releaseBuddy` errors to the caller of `BuddyAllocator_free` instead of silently ignoring them, so upper layers can log or recover from invalid frees.
- **Configurable minimum block size**: currently `min_bucket_size` is derived from `buffer_size / 2^(levels-1)`. Accepting it as an explicit parameter would allow callers to tune internal fragmentation.
- **Thread safety**: adding a mutex around `malloc` and `free` would make the allocator safe for concurrent use with minimal overhead given the O(log n) critical section.
- **Statistics API**: exposing functions for current fragmentation, peak utilization, and allocation count would make the allocator easier to profile in larger systems.
- **Slab layer on top**: grouping small, fixed-size allocations into slabs allocated from the buddy system would reduce internal fragmentation for workloads with many same-sized objects.

## References

- Original course project: https://gitlab.com/grisetti/sistemi_operativi_2020_21/-/tree/master/source/06_memory_allocation/buddy_allocator
- Course slides: https://gitlab.com/grisetti/sistemi_operativi_2020_21/-/blob/master/slides/os_06_memory_allocators.pdf
- Bitmap buddy mapping: https://stackoverflow.com/questions/29071093/how-to-map-bits-from-a-bitmap-to-physical-pages-in-a-buddy-allocator
- Memory management foundations: https://eatplayhate.me/2010/09/04/memory-management-from-the-ground-up-2-foundations/
