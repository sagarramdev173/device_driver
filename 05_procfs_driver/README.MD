# Procfs exercise

Procfs is a virtual filesystem for small, text-oriented kernel status and
control files. This module creates `/proc/procfs_demo`; the kernel routes
userspace `cat` and `echo` operations to the callbacks in `struct proc_ops`.

`proc_create()` owns registration. Save its returned entry and call
`proc_remove()` in module exit. Do not use character-device registration or a
separate userspace test program here: the shell is the complete interface.

## Your task

Complete the two TODOs in `driver.c`:

1. Implement `.proc_read` with `simple_read_from_buffer()` so a second read
   at the same file offset returns EOF.
2. Implement `.proc_write` so it accepts at most `MESSAGE_SIZE - 1` bytes,
   checks `copy_from_user()`, terminates `message`, and returns the accepted
   byte count.

Build and test on the QEMU guest:

```sh
make
sudo insmod driver.ko
cat /proc/procfs_demo
echo 'updated from userspace' | sudo tee /proc/procfs_demo
cat /proc/procfs_demo
sudo rmmod driver
test ! -e /proc/procfs_demo && echo PASS
```

Expected after completing the TODOs: the first `cat` shows the default text,
the second shows the update, and unloading removes the Procfs entry.
