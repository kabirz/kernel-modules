#include <linux/init.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/sched.h>

static __init int test_driver_init(void)
{
	int result = 0;
	char cmd_path[] = "/usr/bin/touch";
	char *cmd_argv[] = { cmd_path, EXECPATH, NULL };
	char *cmd_envp[] = { "HOME=/", "PATH=/sbin:/bin:/usr/bin", NULL };

	result = call_usermodehelper(cmd_path, cmd_argv, cmd_envp,
				     UMH_WAIT_PROC);
	pr_info("test driver init exec! there result of call_usermodehelper is %d\n",
		result);
	pr_info("test driver init exec! the process is \"%s\", pid is %d.\n",
		current->comm, current->pid);
	return result;
}

static __exit void test_driver_exit(void)
{
	int result = 0;
	char cmd_path[] = "/bin/rm";
	char *cmd_argv[] = { cmd_path, EXECPATH, NULL };
	char *cmd_envp[] = { "HOME=/", "PATH=/sbin:/bin:/usr/bin", NULL };

	result = call_usermodehelper(cmd_path, cmd_argv, cmd_envp,
				     UMH_WAIT_PROC);
	pr_info("test driver exit exec! the result of call_usermodehelper is %d\n",
		result);
	pr_info("test driver exit exec! the process is \"%s\",pidis %d \n",
		current->comm, current->pid);
}

module_init(test_driver_init);
module_exit(test_driver_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("kabirz <jxwazxzhp@126.com>");
MODULE_DESCRIPTION("Kernel usermode helper execution example");
MODULE_VERSION("1.0");
