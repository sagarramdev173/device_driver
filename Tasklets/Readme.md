# Tasklets

Tasklets are used to queue up work to be done at a later time. Tasklets can be run in parallel, but the same tasklet cannot be run on multiple CPUs at the same time.
 Also, each tasklet will run only on the CPU that schedules it, to optimize cache usage.
 Since the thread that queued up the tasklet must complete before it can run the tasklet, race conditions are naturally avoided

 Points To Remember
    Tasklets are atomic, so we cannot use sleep() and such synchronization primitives as mutexes, semaphores, etc. from them. But we can use spinlock.
    A tasklet only runs on the same core (CPU) that schedules it.
    Tasklets are executed by the principle of non-preemptive scheduling, one by one, in turn. We can schedule them with two different priorities: normal and high.

We can create a tasklet in Two ways.
    Static Method
    Dynamic Method

tasklet structure

    struct tasklet_struct
    {
        struct tasklet_struct *next;  
        unsigned long state;         
        atomic_t count;               
        void (*func)(unsigned long); 
        unsigned long data;           
    };

Here next - The next tasklet in line for scheduling.
state -This state denotes Tasklet State. TASKLET_STATE_SCHED (Scheduled) or TASKLET_STATE_RUN (Running).
count - It holds a nonzero value if the tasklet is disabled and 0 if it is enabled.
func - This is the main function of the tasklet. Pointer to the function that needs to schedule for execution at a later time.

DECLARE_TASKLET(name, func, data);
    data – Data to be passed to the function “func“.
DECLARE_TASKLET_DISABLED(name, func, data);

Enable and Disable Tasklet
    void tasklet_enable(struct tasklet_struct *t);
    void tasklet_disable(struct tasklet_struct *t);
Schedule the tasklet
    void tasklet_schedule (struct tasklet_struct *t);
    void tasklet_hi_schedule (struct tasklet_struct *t);
    void tasklet_hi_schedule_first(struct tasklet_struct *t);
Kill Tasklet
    void tasklet_kill( struct tasklet_struct *t );
    void tasklet_kill_immediate( struct tasklet_struct *t, unsigned int cpu );


