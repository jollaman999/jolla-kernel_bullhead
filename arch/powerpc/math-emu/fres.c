#include <linux/types.h>
#include <linux/errno.h>
#include <asm/uaccess.h>

int
fres(void *frD, void *frB)
{
#ifdef DEBUG
	printk("%s: %pK %pK\n", __func__, frD, frB);
#endif
	return -ENOSYS;
}
