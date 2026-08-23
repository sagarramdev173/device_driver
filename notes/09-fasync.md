# fasync/SIGIO

Purpose: notify a process asynchronously when a device event occurs.

User setup: `F_SETOWN`, enable `O_ASYNC`/`FASYNC`, install a `SIGIO` handler. Driver uses `.fasync`, `fasync_helper`, and `kill_fasync(&queue, SIGIO, POLL_IN)`.

The signal is only a notification; the application calls `read()` to consume data. On release, call `fasync_helper(-1, file, 0, &queue)` to unregister the file.

Use for occasional simple events and legacy signal-driven applications. Advantages: no polling loop. Limitations: signals can coalesce, handlers are constrained, and `poll`/`epoll` is usually easier to scale and debug.

Lifecycle example: enabling `FASYNC` calls the driver's `.fasync` callback,
which adds the file with `fasync_helper(fd, file, mode, &queue)`. On an event,
the driver changes its state, unlocks any buffer mutex, and calls
`kill_fasync(&queue, SIGIO, POLL_IN)`. The handler should only set a flag or
write to a safe notification fd; normal code then calls `read()`. In
`release()`, `fasync_helper(-1, file, 0, &queue)` removes that file. `-1` and
mode 0 mean cleanup, not a new user fd.

Real-world example: an older serial or modem application enables `SIGIO` so
an arriving character wakes its event loop; the handler records notification
and normal code reads the serial buffer.

Interview Q&A:

- Q: Does `SIGIO` contain the device payload? A: No; it is only a readiness
  notification and events may coalesce.
- Q: Why unregister in `release()`? A: To remove the closing file from the
  async list and prevent later notifications from using stale state.
