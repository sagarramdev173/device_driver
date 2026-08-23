# Character devices

Purpose: expose byte-stream or control operations through `/dev/node`.

Flow: `alloc_chrdev_region` -> `cdev_init`/`cdev_add` -> `class_create` -> `device_create`; VFS dispatches `.open`, `.read`, `.write`, `.ioctl`, `.poll`, `.release` in `struct file_operations`.

Key terms: major identifies the driver class; minor identifies an instance. `file->private_data` stores per-open state; `*off` is the per-file read position.

Read-position example: if the buffer contains `hello` (`available = 5`) and a
read requests 2 bytes, copy `min(count, available - *off)` = 2 bytes, then
advance `*off` from 0 to 2. The next read sees 3 bytes remaining. When
`*off >= available`, the complete message has been consumed. `*off` belongs to
that open file; the shared buffer and readiness flag need synchronization.

Use for serial-like streams, sensors, simple controls. Advantages: standard Unix file API and easy scripting. Limitations: ABI design and user/kernel copies are required.

Pitfalls: use `copy_to_user`/`copy_from_user`, return bytes or negative errno, handle partial reads, protect shared buffers, destroy device before class/cdev/number cleanup.

Interview: “The VFS maps a device node’s major/minor to the registered cdev and calls the driver’s file operations.”

Remember: `read()` returns 0 only when the current file position has reached
the available data (or the driver intentionally reports EOF); returning the
same data forever usually means the driver did not advance `*off`.

Real-world example: a UART driver exposes `/dev/ttyS0`; `write()` queues bytes
for transmission, `read()` returns received bytes, and `poll()` reports when
the receive buffer is non-empty.

Interview Q&A:

- Q: What does the major/minor pair identify? A: Major selects the driver;
  minor selects the device instance handled by that driver.
- Q: Why use `copy_to_user()`? A: Kernel and user memory are separate trust
  domains; the helper validates/accesses the user pointer safely.
