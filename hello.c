#include <linux/init.h>
#include <linux/module.h>
#include <linux/printk.h>
#include <linux/moduleparam.h>
#include <linux/types.h>

#include "linux/ktime.h"
#include "linux/slab.h"
#include "linux/list.h"

MODULE_AUTHOR("Boikivskiy Andriy");
MODULE_DESCRIPTION("'Hello, world' with parameter and timer in Linux Kernel Training");
MODULE_LICENSE("Dual BSD/GPL");

static uint hello_count = 1;
module_param(hello_count, uint, S_IRUGO);
MODULE_PARM_DESC(hello_count, "Amount of 'Hello world' to be printed");

struct my_list_head {
	struct list_head next;
	ktime_t time_before;
	ktime_t time_after;
};

static LIST_HEAD(head);

static int __init hello_init(void)
{
  struct my_list_head *last;
  uint i;

  BUG_ON(hello_count > 10);
    if (hello_count == 0) {
        printk(KERN_WARNING "'hello_count' value is 0\n");
        return 0;
    } else if (hello_count >= 5 && hello_count <= 10) {
        printk(KERN_WARNING "'hello_count'=%i value is between 5 and 10\n", hello_count);
	}

  for (i = 0; i < hello_count; i++) {
    last = kmalloc(sizeof(struct my_list_head), GFP_KERNEL);

    if (i == 7)
      last = NULL;

    if (ZERO_OR_NULL_PTR(last))
      goto km_error;

    last->time_before = ktime_get();
    printk(KERN_INFO "Hello, world!\n");
	last->time_after = ktime_get();

    list_add_tail(&(last->next), &head);
  }

  return 0;

km_error:
  {
    struct my_list_head *md, *tmp;

    printk(KERN_ERR "Not enough memory for element.");
    list_for_each_entry_safe(md, tmp, &head, next) {
      list_del(&md->next);
      kfree(md);
    }
    BUG();
    return -ENOMEM;
  }
}

static void __exit hello_exit(void)
{
  struct my_list_head *md, *tmp;

  list_for_each_entry_safe(md, tmp, &head, next) {
		pr_info("Start time: %lld\n", md->time_before);
		pr_info("Execution time: %lld\n", md->time_after - md->time_before);
		list_del(&md->next);
		kfree(md);
  }
  BUG_ON(!list_empty(&head));
}

module_init(hello_init);
module_exit(hello_exit);
