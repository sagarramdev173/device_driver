# ioctl

Purpose: driver control plane for discrete commands that are not byte-stream reads/writes: reset, mode, configuration, status.

APIs: `.unlocked_ioctl`, `_IO`, `_IOR`, `_IOW`, `_IOWR`, `copy_to_user`, `copy_from_user`.

Define a magic value and command numbers. Validate the command, copy through the user pointer, protect shared state, and return `-ENOTTY` for unknown commands and `-EFAULT` for failed copies.

Use for typed configuration and queries; use `read`/`write` for ordinary data. Limitations: ioctl is a long-lived ABI and must handle 32/64-bit compatibility carefully.

Pitfalls: dereferencing `arg` directly; passing `&arg` instead of `arg`; ignoring copy return values; holding a mutex across user copies unnecessarily.

Pointer example: `arg` is the user address encoded as an integer. For a local
value use `copy_from_user(&value, (int __user *)arg, sizeof(value))`; do not use
`&arg`, which points to the kernel's syscall-argument variable. For GET,
snapshot shared state under the lock, unlock, then
`copy_to_user((int __user *)arg, &value, sizeof(value))`. User copies may fault
and sleep, so do not perform them under a spinlock.

Command encoding: `_IOW(type, number, int)` means user-to-driver data;
`_IOR(type, number, int)` means driver-to-user data. The encoded `cmd` is not
just the magic byte, so dispatch on the complete macro value and return
`-ENOTTY` for an unknown command. The direction describes data flow from the
driver ABI's point of view, not whether the command is conceptually a read.

Real-world example: an RTC driver uses ioctls to set an alarm or query the
clock status, while `read()` returns time/event data and is not overloaded with
configuration strings.

Interview Q&A:

- Q: Why not use `write("reset")` for control? A: Ioctl gives a typed,
  versionable command ABI and separates control from stream data.
- Q: What must every ioctl validate? A: The command, argument size/layout,
  user pointer, access permissions, and any value ranges.
