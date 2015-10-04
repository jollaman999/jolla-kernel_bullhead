/*
 * linux/fs/hfsplus/xattr_trusted.c
 *
 * Vyacheslav Dubeyko <slava@dubeyko.com>
 *
 * Handler for trusted extended attributes.
 */

#include "hfsplus_fs.h"
#include "xattr.h"

static int hfsplus_trusted_getxattr(struct dentry *dentry, const char *name,
					void *buffer, size_t size, int type)
{
	char xattr_name[HFSPLUS_ATTR_MAX_STRLEN + 1] = {0};
	size_t len = strlen(name);

	if (!strcmp(name, ""))
		return -EINVAL;

	if (len + XATTR_TRUSTED_PREFIX_LEN > HFSPLUS_ATTR_MAX_STRLEN)
		return -EOPNOTSUPP;

	strcpy(xattr_name, XATTR_TRUSTED_PREFIX);
	strcpy(xattr_name + XATTR_TRUSTED_PREFIX_LEN, name);

	return hfsplus_getxattr(dentry, xattr_name, buffer, size);
}

static int hfsplus_trusted_setxattr(struct dentry *dentry, const char *name,
		const void *buffer, size_t size, int flags, int type)
{
	char xattr_name[HFSPLUS_ATTR_MAX_STRLEN + 1] = {0};
	size_t len = strlen(name);

	if (!strcmp(name, ""))
		return -EINVAL;

	if (len + XATTR_TRUSTED_PREFIX_LEN > HFSPLUS_ATTR_MAX_STRLEN)
		return -EOPNOTSUPP;

	strcpy(xattr_name, XATTR_TRUSTED_PREFIX);
	strcpy(xattr_name + XATTR_TRUSTED_PREFIX_LEN, name);

	return hfsplus_setxattr(dentry, xattr_name, buffer, size, flags);
}

const struct xattr_handler hfsplus_xattr_trusted_handler = {
	.prefix	= XATTR_TRUSTED_PREFIX,
	.get	= hfsplus_trusted_getxattr,
	.set	= hfsplus_trusted_setxattr,
};
