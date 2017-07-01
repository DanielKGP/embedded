#define DECLARE_TASKLET(name,func,data)
struct tasklet_struct name = {NULL,0,ATOMIC_INIT(0),func,data}

#define DECLARE_TASKLET_DISABLED(name,func,data)
struct tasklet_struct name = {NULL,0,ATOMIC_INIT(1),func,data}

struct tasklet_struct{
  struct tasklet_struct *next;
  unsigned long state;
  atomic_t count;
  void (*func)(unsigned long);
  unsigned long data;
};

void tasklet_init(struct tasklet_struct *t,void(*func)(unsigned long),unsigned long data)
{
  t->next = NULL;
  t->state = 0;
  atomic_set(&t->count,0);
  t->func = func;
  t->data = data;
}

void tasklet_handler(unsigned long data)

static inline void tasklet_disable(struct tasklet_struct *t)
{
  tasklet_disable_nosync(t);
  tasklet_unlock_wait(t);
  smp_mb();
}

static inline void tasklet_disable_nosync(struct tasklet_struct *t)
{
  atomic_inc(&t->count);
  smp_mb__after_atomic_inc();
}

void tasklet_kill(struct tasklet_struct *t)
{
  if(in_interrupt())
    printk("Attempt to kill tasklet from interrupt\n");
  while(test_and_set_bit(TASKLET_STATE_SCHED,&t->state))
  {
    do
      yield();
    while(test_bit(TASKLET_STATE_SCHED,&t->state));
  }
  tasklet_unlock_wait(t);
  clear_bit(TASKLET_STATE_SCHED,&t->state);
}

void tasklet_kill_immediate(struct tasklet_struct *t,unsigned int cpu)
{
  struct tasklet_struct **i;
  BUG_ON(cpu_online(cpu));
  BUG_ON(test_bit(TASKLET_STATE_RUN,&t->state));
  if(!test_bit(TASKLET_STATE_SCHED,&t->state))
    return;
  for(i=&per_cpu(tasklet_vec,cpu).head;*i;i=&(*i)->next)
  {
    if(*i == t)
    {
      *i = t ->next;
      if(*i == NULL)
        per_cpu(tasklet_vec,cpu).tail = i;
      return;
    }
  }
  BUG();
}

struct cpu workqueue struct
{
  spinlock_t lock;
  long remove_sequence;
  long insert_sequence;
  struct list_head worklist;
  wait_queue_head_t more_work;
  wait_queue_head_t work_done;
  struct workqueue_struct *wq;
  task_t *thread;
  int run_depth;
};

struct work_struct
{
  unsigned long pending;
  struct list_head entry;
  void (*func)(void *);
  void *data;
  void *wq_data;
  struct timer_list timer;
};

#define DECLARE_WORK(n,f)
struct work_struct n = __WORK_INITIALIZER(n,f)

#define DECLARE_DELAYED_WORK(n,f)
struct delayed_work n = __DELAYED_WORK_INITIALIZER(n,f)

#define __WORK_INITIALIZER(n,f){
    .data = WORK_DATA_INIT(),
    .entry = { &(n).entry,&(n).entry},
    .func = (f),
    __WORK_INIT_LOCKDEP_MAP(#n,&(n))
  }

#define __DELAYED_WORK_INITIALIZER(n,f){
    .work = __WORK_INITIALIZER((n).work,(f)),
    .timer = TIMER_INITIALIZER(NULL,0,0),
  }

#define INIT_WORK(work,func)
    do{
      (_work)->data = (atomic_long_t) WORK_DATA_INIT();
      INIT_LIST_HEAD(&(_work)->entry);
      PREPARE_WORK((_work),(_func));
    }while(0)

static inline void INIT_LIST_HEAD(struct list_head *list)
{
  list->next = list;
  list->prev = list;
}

#define PREPARE_WORK(work,func)
    do{
      (_work)->func = (_func);
    }while(0)

struct workqueue_struct
{
  struct cpu_workqueue_struct *cpu_wq;
  struct list_head list;
  const char *name;
  int singlethread;
  int freezeable;
#ifdef CONFIG_LOCKDEP
  struct lockdep_map lockdep_map;
#endif
};

#ifdef CONFIG_LOCKDEP
#define __create_workqueue(name,singlethread,freezeable)
({
  static struct lock_class_key __key;
  const char *__lock_name;

  if(__builtin_constant_p(name))
     __lock_name = (name);
  else
     __lock_name = #name;

  __create_workqueue_key((name),(singlethread),(freezeable),&__key,__lock_name);

})
#endif
