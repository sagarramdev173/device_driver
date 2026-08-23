# Module lifecycle

Purpose: load kernel code dynamically without rebuilding the kernel.

Key APIs: `module_init(init_fn)`, `module_exit(exit_fn)`, `MODULE_LICENSE("GPL")`, `insmod`, `rmmod`.

Flow: init allocates/registers resources; on failure, unwind in reverse order. Exit releases every resource. Use `goto` labels for shared cleanup paths.

Use cases: optional drivers, development, platform modules. Advantages: small iteration cost and no reboot. Limitations: kernel-version/build-tree compatibility; unsigned out-of-tree modules taint the kernel.

Pitfalls: wrong vermagic, missing cleanup, unloading with active users, calling cleanup in the wrong order.

Interview: “A module init function acquires resources and registers interfaces; exit unregisters interfaces and frees resources in reverse order. Every partial-init path must release only what was acquired.”

Real-world example: a USB Wi-Fi driver can be loaded when the adapter is
plugged in and unloaded when it is removed; its init registers the USB driver
and its probe allocates per-adapter state.

Interview Q&A:

- Q: Why use `goto` cleanup labels? A: They centralize reverse-order cleanup
  for several failure paths without duplicating unlock/free code.
- Q: Why can an out-of-tree module taint the kernel? A: The kernel cannot
  treat unsigned or external code as fully trusted for support and diagnosis.
