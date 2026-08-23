# vmalloc/vzalloc

`vmalloc(size)` returns one virtually contiguous CPU range backed by physically scattered pages; free with `vfree`. `vzalloc` additionally zeroes memory.

Use for large software-only buffers, tables, or data where physical contiguity is unnecessary. Advantages: large allocations are less constrained by physical fragmentation. Costs: slower allocation, page-table/TLB overhead, and it may sleep.

Do not use in interrupt/atomic context. Do not pass a vmalloc pointer directly to hardware or convert it with `virt_to_phys`; use the DMA API and mapping functions.

Interview: “kmalloc is physically contiguous for normal objects; vmalloc is only virtually contiguous and is suitable when hardware does not need physical contiguity.”

Example choice: a 64-byte device-state structure uses `kzalloc`, not
`vmalloc`; `vmalloc` adds page-table overhead and is slower for small objects.
Several-megabyte software lookup data may use `vzalloc` when physical layout
does not matter. A device descriptor ring or transfer buffer must use the DMA
API instead, even if a `vmalloc` pointer looks contiguous to the CPU.

Real-world example: a tracing driver may allocate a large software-only table
of event metadata with `vzalloc`; a GPU or NIC buffer instead uses the device's
DMA allocation/mapping API.

Interview Q&A:

- Q: Is a `vmalloc` pointer physically contiguous? A: No; only the CPU virtual
  address range is contiguous.
- Q: Why is `vmalloc` unsuitable for a 64-byte object? A: It adds page mapping
  overhead; `kzalloc` is faster and appropriate for small state.
