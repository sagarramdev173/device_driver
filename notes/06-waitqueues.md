# Waitqueues

Purpose: sleep efficiently until a driver condition becomes true.

APIs: `init_waitqueue_head`, `wait_event_interruptible`, `wake_up_interruptible`.

Reader flow: wait on a predicate such as `data_ready`; writer updates the protected buffer, changes the predicate, then wakes readers. Check the wait return value (`-ERESTARTSYS` on signal).

Use for blocking reads, hardware completion, input arrival, and producer/consumer drivers. Advantage: no busy looping. Limitations: the predicate and state must be synchronized; wakeups do not replace the condition.

Pitfalls: sleeping while holding a spinlock; forgetting to wake; clearing readiness before all bytes are consumed; unloading with blocked users.

The wait predicate is the source of truth:

```c
wait_event_interruptible(read_queue, data_ready);
```

The writer must make the state true before waking. A wakeup without a changed
predicate is harmless but does not make the read safe. Because the wait can be
interrupted, handle its return value before touching the buffer. Never hold a
mutex or spinlock while waiting; lock after the wait returns and recheck the
state if the design permits races.

Teardown warning: a module must not free its waitqueue or buffer while a task
can still be sleeping on it. A production driver adds a shutdown/removed
predicate, wakes all waiters during removal, and makes blocked operations
return `-ENODEV`; the small exercise assumes readers are stopped first.

Real-world example: a GPIO button driver puts a process to sleep until an IRQ
records a button event, then wakes the reader to consume the event.

Interview Q&A:

- Q: Why is the predicate required? A: Wakeups are hints; the predicate tells
  the awakened task whether the requested condition is actually true.
- Q: Why use `wait_event_interruptible()`? A: Signals can interrupt the sleep,
  allowing Ctrl-C or termination instead of an unkillable wait.
