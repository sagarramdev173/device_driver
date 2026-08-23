# kmalloc, kzalloc, and GFP flags

`kmalloc(size, flags)` returns virtually and physically contiguous memory; free with `kfree`. `kzalloc` has the same behavior but zeroes the allocation.

`GFP_KERNEL` may sleep and is for process/sleepable context such as init, read, write, ioctl, probe, and workqueues. `GFP_ATOMIC` cannot sleep and is for interrupt/timer context or while holding a spinlock; it can fail more easily.

Use `kzalloc` for structures whose fields should start at zero and to avoid exposing uninitialized data. Check `NULL` and return `-ENOMEM`.

Pitfalls: using GFP_KERNEL in atomic context; assuming kmalloc memory is zeroed; treating a CPU pointer as a DMA address; mismatching `vmalloc`/`kfree`.

Context decision: module init, `open`, `read`, `write`, `ioctl`, probe, and a
workqueue callback may sleep, so use `GFP_KERNEL`. A timer/interrupt callback
or code holding a spinlock cannot sleep, so use `GFP_ATOMIC` only when the
allocation cannot be deferred. Prefer allocating before entering atomic
context or queueing work; atomic allocations have fewer reclaim options and
can fail more easily.

`kzalloc(sizeof(*state), GFP_KERNEL)` is useful for a new state structure
because all fields start at zero. `kmalloc` is fine when every byte is
immediately initialized. Both are released with `kfree`.

Real-world example: a PCI driver's `probe()` allocates a zeroed per-device
structure with `devm_kzalloc()`/`kzalloc`, initializes locks and queues, and
frees it during remove if it is not device-managed.

Interview Q&A:

- Q: Why can `GFP_KERNEL` fail even when memory appears available? A: The
  requested allocation constraints and slab/physical fragmentation still may
  prevent a suitable block.
- Q: What is the matching release for `kmalloc`? A: `kfree`; do not use
  `vfree` or `__free_page`.
