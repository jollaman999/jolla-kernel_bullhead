/*
 * linux/fs/hfsplus/xattr_trusted.c
 *
 * Vyacheslav Dubeyko <slava@dubeyko.com>
 *
 * Handler for storing security labels as extended attributes.
 */

#include <linux/security.h>
#include "hfsplus_fs.h"
#include "xattr.h"

static int hfsplus_security_getxattr(const struct xattr_handler *handler,
				     struct dentry *dentry, const char *name,
				     void *buffer, size_t size)
{
	char xattr_name[HFSPLUS_ATTR_MAX_STRLEN + 1] = {0};
	size_t len = strlen(name);

	if (!strcmp(name, ""))
		return -EINVAL;

	if (len + XATTR_SECURITY_PREFIX_LEN > HFSPLUS_ATTR_MAX_STRLEN)
		return -EOPNOTSUPP;

	strcpy(xattr_name, XATTR_SECURITY_PREFIX);
	strcpy(xattr_name + XATTR_SECURITY_PREFIX_LEN, name);

	return hfsplus_getxattr(dentry, xattr_name, buffer, size);
}

static int hfsplus_security_setxattr(const struct xattr_handler *handler,
				     struct dentry *dentry, const char *name,
				     const void *buffer, size_t size, int flags)
{
	char xattr_name[HFSPLUS_ATTR_MAX_STRLEN + 1] = {0};
	size_t len = strlen(name);

	if (!strcmp(name, ""))
		return -EINVAL;

	if (len + XATTR_SECURITY_PREFIX_LEN > HFSPLUS_ATTR_MAX_STRLEN)
		return -EOPNOTSUPP;

	strcpy(xattr_name, XATTR_SECURITY_PREFIX);
	strcpy(xattr_name + XATTR_SECURITY_PREFIX_LEN, name);

	return hfsplus_setxattr(dentry, xattr_name, buffer, size, flags);
}

static int hfsplus_initxattrs(struct inode *inode,
				const struct xattr *xattr_array,
				void *fs_info)
{
	const struct xattr *xattr;
	char xattr_name[HFSPLUS_ATTR_MAX_STRLEN + 1] = {0};
	size_t xattr_name_len;
	int err = 0;

	for (xattr = xattr_array; xattr->name != NULL; xattr++) {
		xattr_name_len = strlen(xattr->name);

		if (xattr_name_len == 0)
			continue;

		if (xattr_name_len + XATTR_SECURITY_PREFIX_LEN >
				HFSPLUS_ATTR_MAX_STRLEN)
			return -EOPNOTSUPP;

		strcpy(xattr_name, XATTR_SECURITY_PREFIX);
		strcpy(xattr_name +
			XATTR_SECURITY_PREFIX_LEN, xattr->name);
		memset(xattr_name +
			XATTR_SECURITY_PREFIX_LEN + xattr_name_len, 0, 1);

		err = __hfsplus_setxattr(inode, xattr_name,
					xattr->value, xattr->value_len, 0);
		if (err)
			break;
	}
	return err;
}

int hfsplus_init_security(struct inode *inode, struct inode *dir,
				const struct qstr *qstr)
{
	return security_inode_init_security(inode, dir, qstr,
					&hfsplus_initxattrs, NULL);
}

const struct xattr_handler hfsplus_xattr_security_handler = {
	.prefix	= XATTR_SECURITY_PREFIX,
	.get	= hfsplus_security_getxattr,
	.set	= hfsplus_security_setxattr,
};
