# poll/select

Purpose: let user space wait for readiness on one or many file descriptors without busy-waiting.

Driver API: `.poll = demo_poll`, `poll_wait(file, &queue, wait)`, return `POLLIN | POLLRDNORM` when readable.

`poll_wait` registers the caller; it does not itself sleep or wake anyone. The producer changes the readiness condition and calls `wake_up_interruptible`.

Use for event loops, serial/input devices, sensors, and multiple devices. Advantage: one thread can wait on devices and sockets. Prefer `epoll` for very large fd sets.

Interview: “The callback registers a waitqueue and reports current readiness; a later wakeup causes the kernel to recheck it.”

Exact flow:

```text
poll_wait() registers the task -> callback returns 0 -> task sleeps
write updates data_ready -> wake_up_interruptible()
kernel calls .poll again -> POLLIN is returned -> user read()s the data
```

`poll_wait()` is not a wakeup and does not itself block. The readiness test
must describe what `read()` can consume now. Return `POLLIN | POLLRDNORM` for
normal readable data; return 0 when a blocking read would wait. A readiness
flag must be cleared only after the complete message/event has been consumed.

Real-world example: a monitoring daemon waits in one `poll()` call for a
serial port, a GPIO event device, and a control socket instead of creating a
busy loop for each source.

Interview Q&A:

- Q: Does `.poll` transfer data? A: No; it reports readiness. User space still
  calls `read()` or `write()` to transfer data.
- Q: What does a driver do when no data is ready? A: Register its waitqueue
  with `poll_wait()` and return no readable flag; the kernel may sleep the task.
