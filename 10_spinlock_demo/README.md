# Spinlock demo

This small module has one shared counter:

- the timer callback updates it in atomic context;
- the workqueue callback reads it in process context;
- both use `spin_lock_irqsave()` so the shared value is consistent.

The timer callback is interrupt-like atomic context, not a hardware IRQ. A
real hard-IRQ handler follows the same no-sleep rule. The workqueue is used for
the slower/logging part.

Build and run with the GCC 11 toolset used by this UEK kernel:

```sh
scl enable gcc-toolset-11 -- make
sudo insmod spinlock_demo.ko
dmesg | tail -n 10
sudo rmmod spinlock_demo
dmesg | tail -n 10
```

Expected output includes increasing `process context read count=` messages and
`unloaded safely`.
