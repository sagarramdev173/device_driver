# Page allocator

Purpose: allocate memory in page-sized units and work with `struct page`.

APIs: `alloc_page(GFP_KERNEL)`, `alloc_pages(flags, order)`, `page_address(page)`, `__free_page`, `__free_pages`. `order` allocates `2^order` physically contiguous pages.

Use for page-granular buffers, mappings, and lower-level memory work. High-order allocations can fail due to fragmentation. `page_address` gives a CPU virtual address for directly addressable pages; highmem may require `kmap_local_page`.

Pitfalls: confusing `struct page *` with a byte pointer; forgetting cleanup on mapping failure; using page memory directly for DMA instead of the DMA API.

`alloc_page()` returns metadata (`struct page *`), not a writable character
buffer. `page_address(page)` gives the CPU virtual address for directly
addressable memory; use that address for `memcpy`/`snprintf`. The matching
release is `__free_page(page)`, not `kfree(page_address(page))`.

`alloc_pages(GFP_KERNEL, order)` returns `2^order` physically contiguous
pages. Higher orders are harder to satisfy because one contiguous physical
range is required. For a small object use `kmalloc`; use the page allocator
when page granularity or a page mapping is part of the design.

Page memory is not automatically a DMA buffer. Hardware access requires the
DMA API (`dma_alloc_coherent` or `dma_map_single`), which handles device
addressing and cache/IOMMU rules.

Real-world example: a network driver allocates pages for receive data, maps
them for DMA, and recycles the pages after the device finishes writing a
packet; page granularity avoids one allocation per small fragment.

Interview Q&A:

- Q: What does `order = 3` mean? A: Eight physically contiguous pages
  (`2^3`), not three pages.
- Q: Why can high-order allocation fail? A: It needs one contiguous physical
  range, which fragmentation may make unavailable even with free pages.
