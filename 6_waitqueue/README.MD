# WAITQUEUE 
When you write a Linux Driver Module or Kernel Program, Some processes should wait or sleep for some event. There are several ways of handling sleeping and waking up in Linux, each suited to different needs. Waitqueue is also one of the methods to handle that case.

In Linux, a wait queue is managed by means of a “wait queue head,” a structure of type wait_queue_head_t, which is defined in <linux/wait.h>. A wait queue head can be defined and initialized statically with:
``` C
DECLARE_WAIT_QUEUE_HEAD(name);
```
or dynamicly as follows:

``` C
wait_queue_head_t my_queue;
init_waitqueue_head(&my_queue);
```

## Simple Sleeping
When a process sleeps, it does so in expectation that some condition will become true in the future.

``` C
wait_event(queue, condition)
wait_event_interruptible(queue, condition)
wait_event_timeout(queue, condition, timeout)
wait_event_interruptible_timeout(queue, condition, timeout)
```
In all of the above forms, queue is the wait queue head to use. Notice that it is passed “by value.” The condition is an arbitrary boolean expression that is evaluated by the macro before and after sleeping; until condition evaluates to a true value, the process continues to sleep

`wait_event` is used when your process is put into an uninterruptible sleep 
`wait_event_interruptible` is used when your process is put into an interruptible sleep
`wait_event_timeout` is used when your process is put into an uninterruptible sleep with a timeout
`wait_event_interruptible_timeout` is used when your process is put into an interruptible sleep with a timeout

### Return Value
 `wait_event_interruptible`, which can be interrupted by signals. This version returns an integer value that you should check; a nonzero value means your sleep was interrupted by some sort of signal, and your driver should probably return `-ERESTARTSYS`

