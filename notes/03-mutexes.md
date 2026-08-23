# Mutexes

Purpose: serialize sleepable access to shared state.

APIs: `DEFINE_MUTEX`, `mutex_init`, `mutex_lock`, `mutex_unlock`, `mutex_lock_interruptible`.

Use in `read`, `write`, `ioctl`, open, probe, and workqueue context. A mutex may sleep, so do not use it in interrupt/timer context.

Advantages: simple ownership semantics and safe protection around code that can sleep. Pitfalls: always unlock on every path; avoid holding it across slow or unnecessary work. `copy_to_user` can sleep, so design the critical section deliberately.

Interview: “I use a mutex when the context may sleep and multiple threads access shared driver state; the owner can sleep while holding it.”

Real-world example: two processes writing configuration to the same I2C
sensor driver serialize access to the shared mode and sampling-rate fields
with a mutex.

Interview Q&A:

- Q: What happens if a second task tries to lock a held mutex? A: It sleeps
  until the owner unlocks, so it does not burn CPU.
- Q: Why not use a mutex in an interrupt handler? A: Interrupt context cannot
  sleep; use a spinlock or defer the work.
