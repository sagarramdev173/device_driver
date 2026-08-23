# Spinlocks

Purpose: protect short critical sections in atomic contexts or between interrupt and process context.

APIs: `spin_lock`, `spin_unlock`, `spin_lock_irqsave`, `spin_unlock_irqrestore`.

Use when a lock holder must not sleep, especially interrupt/timer callbacks. The waiter busy-spins, so hold it briefly.

Never call `copy_to_user`, `copy_from_user`, `kmalloc(GFP_KERNEL)`, or other sleeping code while holding a spinlock.

Advantages: valid in interrupt context. Limitations: wastes CPU while contended and can deadlock with interrupt-preemption ordering.

Interview: “Spinlocks protect short atomic sections; mutexes protect sleepable sections. I use irqsave when local interrupts could race with the protected state.”

Real-world example: a network driver's interrupt handler puts received packet
descriptors on a ring while process-context code removes them; a spinlock
protects the short ring-index update.

Interview Q&A:

- Q: Why keep a spinlocked section short? A: Contenders spin and consume CPU,
  and long holds increase interrupt and scheduling latency.
- Q: Why use `spin_lock_irqsave()`? A: It disables local interrupts and saves
  the prior state, preventing a same-CPU interrupt from re-entering the lock.
