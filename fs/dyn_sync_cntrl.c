/*
 * Author: Paul Reioux aka Faux123 <reioux@gmail.com>
 *
 * Copyright 2013 Paul Reioux
 * Copyright 2012 Paul Reioux
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#include <linux/module.h>
#include <linux/kobject.h>
#include <linux/sysfs.h>
#include <linux/mutex.h>
#include <linux/writeback.h>

#include <linux/dyn_sync_cntrl.h>

#define DYN_FSYNC_VERSION_MAJOR 1
#define DYN_FSYNC_VERSION_MINOR 1

/*
 * fsync_mutex protects dyn_fsync_active during pm suspend / resume
 * transitions
 */
static DEFINE_MUTEX(fsync_mutex);
bool dyn_fsync_active __read_mostly = true;

extern void dyn_fsync_suspend_actions(void);

static ssize_t dyn_fsync_active_show(struct kobject *kobj,
		struct kobj_attribute *attr, char *buf)
{
	return sprintf(buf, "%u\n", (dyn_fsync_active ? 1 : 0));
}

static ssize_t dyn_fsync_active_store(struct kobject *kobj,
		struct kobj_attribute *attr, const char *buf, size_t count)
{
	unsigned int data;

	if(sscanf(buf, "%u\n", &data) == 1) {
		if (data == 1) {
			pr_info("%s: dynamic fsync enabled\n", __FUNCTION__);
			dyn_fsync_active = true;
		}
		else if (data == 0) {
			pr_info("%s: dyanamic fsync disabled\n", __FUNCTION__);
			dyn_fsync_active = false;
		}
		else
			pr_info("%s: bad value: %u\n", __FUNCTION__, data);
	} else
		pr_info("%s: unknown input!\n", __FUNCTION__);

	return count;
}

static ssize_t dyn_fsync_version_show(struct kobject *kobj,
		struct kobj_attribute *attr, char *buf)
{
	return sprintf(buf, "version: %u.%u by faux123\n",
		DYN_FSYNC_VERSION_MAJOR,
		DYN_FSYNC_VERSION_MINOR);
}

static struct kobj_attribute dyn_fsync_active_attribute =
	__ATTR(Dyn_fsync_active, 0666,
		dyn_fsync_active_show,
		dyn_fsync_active_store);

static struct kobj_attribute dyn_fsync_version_attribute =
	__ATTR(Dyn_fsync_version, 0444, dyn_fsync_version_show, NULL);

static struct attribute *dyn_fsync_active_attrs[] =
	{
		&dyn_fsync_active_attribute.attr,
		&dyn_fsync_version_attribute.attr,
		NULL,
	};

static struct attribute_group dyn_fsync_active_attr_group =
	{
		.attrs = dyn_fsync_active_attrs,
	};

static struct kobject *dyn_fsync_kobj;

void dyn_fsync_suspend(void)
{
	mutex_lock(&fsync_mutex);
	/* flush all outstanding buffers */
	if (dyn_fsync_active)
		dyn_fsync_suspend_actions();
	mutex_unlock(&fsync_mutex);

	pr_info("%s: flushing work finished.\n", __FUNCTION__);
}

static int __init dyn_fsync_init(void)
{
	int sysfs_result;

	dyn_fsync_kobj = kobject_create_and_add("dyn_fsync", kernel_kobj);
	if (!dyn_fsync_kobj) {
		pr_err("%s dyn_fsync kobject create failed!\n", __FUNCTION__);
		return -ENOMEM;
        }

	sysfs_result = sysfs_create_group(dyn_fsync_kobj,
			&dyn_fsync_active_attr_group);

        if (sysfs_result) {
		pr_info("%s dyn_fsync sysfs create failed!\n", __FUNCTION__);
		kobject_put(dyn_fsync_kobj);
	}
	return sysfs_result;
}

static void __exit dyn_fsync_exit(void)
{
	if (dyn_fsync_kobj != NULL)
		kobject_put(dyn_fsync_kobj);
}

module_init(dyn_fsync_init);
module_exit(dyn_fsync_exit);
