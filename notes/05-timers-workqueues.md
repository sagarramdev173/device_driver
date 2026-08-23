# Timers and workqueues

Timers defer a callback until a later jiffy/time; the callback runs in atomic/softirq-like context. APIs: `timer_setup`, `mod_timer`, `del_timer_sync`.

Workqueues defer work to a kernel worker thread (process context). APIs: `INIT_WORK`, `schedule_work`, `cancel_work_sync`.

Use a timer for lightweight timing and a workqueue for work that may sleep or is too heavy for timer context. A common pattern is timer -> schedule work.

On unload, use `del_timer_sync` first, then `cancel_work_sync`, so no callback can schedule new work while the work is being canceled.

Pitfalls: sleeping in timer callbacks; using freed state from pending work; non-synchronous deletion during unload.

Why the removal order matters: `del_timer()` prevents future timer expiry but
does not wait for a callback already running on another CPU. `del_timer_sync()`
waits for that callback to finish. Only then is it safe to cancel the work it
may have scheduled. `cancel_work_sync()` waits for running work and removes
pending work; it is still useful even if the timer was deleted because work may
already be queued.

Real-world example: an Ethernet driver can use a timer to detect a transmit
timeout, then queue work to reset hardware because register access and reset
steps may sleep.

Interview Q&A:

- Q: Which context runs a timer callback? A: Atomic/softirq-like context; it
  must not sleep.
- Q: Which context runs workqueue code? A: A kernel worker thread in process
  context, so it may sleep and use `GFP_KERNEL`.
