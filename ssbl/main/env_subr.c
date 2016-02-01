/***************************************************************************
 *     Copyright (c) 2012-2013, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************/

#include "lib_types.h"
#include "lib_string.h"
#include "lib_queue.h"
#include "lib_malloc.h"
#include "lib_printf.h"

#include "env_subr.h"
#include "nvram_subr.h"

#include "error.h"
#include "bolt.h"

/*  *********************************************************************
    *  Types
    ********************************************************************* */

struct bolt_envvar_s {
	queue_t qb;
	int flags;
	char *name;
	char *value;
	/* name and value go here */
};

/*  *********************************************************************
    *  Globals
    ********************************************************************* */

queue_t env_envvars = { &env_envvars, &env_envvars };

/*  *********************************************************************
    *  env_findenv(name)
    *
    *  Locate an environment variable in the in-memory list
    *
    *  Input parameters:
    *      name - name of env var to find
    *
    *  Return value:
    *      struct bolt_envvar_s pointer, or NULL if not found
    ********************************************************************* */

static struct bolt_envvar_s *env_findenv(const char *name)
{
	queue_t *qb;
	struct bolt_envvar_s *env;

	for (qb = env_envvars.q_next; qb != &env_envvars; qb = qb->q_next) {
		env = (struct bolt_envvar_s *) qb;
		if (strcmp(env->name, name) == 0)
			break;
	}

	if (qb == &env_envvars)
		return NULL;

	return (struct bolt_envvar_s *) qb;

}

/*  *********************************************************************
    *  env_enum(idx,name,namelen,val,vallen)
    *
    *  Enumerate environment variables.  This routine locates
    *  the nth environment variable and copies its name and value
    *  to user buffers.
    *
    *  The namelen and vallen variables must be preinitialized to
    *  the maximum size of the output buffer.
    *
    *  Input parameters:
    *      idx - variable index to find (starting with zero)
    *      name,namelen - name buffer and length
    *      val,vallen - value buffer and length
    *
    *  Return value:
    *      0 if ok
    *      else error code
    ********************************************************************* */

int env_enum(int idx, char *name, int *namelen, char *val, int *vallen)
{
	queue_t *qb;
	struct bolt_envvar_s *env;

	for (qb = env_envvars.q_next; qb != &env_envvars; qb = qb->q_next) {
		if (idx == 0)
			break;
		idx--;
	}

	if (qb == &env_envvars)
		return BOLT_ERR_ENVNOTFOUND;
	env = (struct bolt_envvar_s *) qb;

	*namelen = xstrncpy(name, env->name, *namelen);
	*vallen = xstrncpy(val, env->value, *vallen);

	return 0;

}

/*  *********************************************************************
    *  env_envtype(name)
    *
    *  Return the type of the environment variable
    *
    *  Input parameters:
    *      name - name of environment variable
    *
    *  Return value:
    *      flags, or <0 if error occured
    ********************************************************************* */
int env_envtype(const char *name)
{
	struct bolt_envvar_s *env;

	env = env_findenv(name);

	if (env)
		return env->flags;

	return BOLT_ERR_ENVNOTFOUND;
}

/*  *********************************************************************
    *  env_delenv(name)
    *
    *  Delete an environment variable
    *
    *  Input parameters:
    *      name - environment variable to delete
    *
    *  Return value:
    *      0 if ok
    *      else error code
    ********************************************************************* */

int env_delenv(const char *name)
{
	struct bolt_envvar_s *env;

	env = env_findenv(name);

	if (!env)
		return 0;

	if (!(env->flags & ENV_FLG_READONLY)) {
		q_dequeue((queue_t *) env);
		KFREE(env);
		return 0;
	}

	return BOLT_ERR_ENVNOTFOUND;
}

/*  *********************************************************************
    *  env_getenv(name)
    *
    *  Retrieve the value of an environment variable
    *
    *  Input parameters:
    *      name - name of environment variable to find
    *
    *  Return value:
    *      value, or NULL if variable is not found
    ********************************************************************* */

char *env_getenv(const char *name)
{
	struct bolt_envvar_s *env;

	env = env_findenv(name);

	if (env)
		return env->value;

	return NULL;
}

/*  *********************************************************************
    *  env_getval(name)
    *
    *  Retrieve the integer value of an environment variable.
    *
    *  Input parameters:
    *      name - name of environment variable to find
    *
    *  Return value:
    *      integer value, or -1 if variable is not found
    ********************************************************************* */

int env_getval(const char *name)
{
	char *s;
	int val = -1;

	s = env_getenv(name);
	if (s)
		val = lib_atoi(s);

	return val;
}

/*  *********************************************************************
    *  env_setenv(name,value,flags)
    *
    *  Set the value of an environment variable
    *
    *  Input parameters:
    *      name - name of variable
    *      value - value of variable
    *      flags - flags for variable (ENV_FLG_xxx)
    *
    *  Return value:
    *      0 if ok
    *      else error code
    ********************************************************************* */

int env_setenv(const char *name, const char *value, int flags)
{
	struct bolt_envvar_s *env;
	int namelen;

	env = env_findenv(name);
	if (env) {
		if (!(flags & ENV_FLG_ADMIN)) {
			if (env->flags & ENV_FLG_READONLY)
				return BOLT_ERR_ENVREADONLY;
		}
		q_dequeue((queue_t *) env);
		KFREE(env);
	}

	namelen = strlen(name);

	env = KMALLOC(sizeof(struct bolt_envvar_s) +
		namelen + 1 + strlen(value) + 1, 0);

	if (!env)
		return BOLT_ERR_NOMEM;

	env->name = (char *)(env + 1);
	env->value = env->name + namelen + 1;
	env->flags = (flags & ENV_FLG_MASK);

	strcpy(env->name, name);
	strcpy(env->value, value);

	q_enqueue(&env_envvars, (queue_t *) env);

	return 0;
}

/*  *********************************************************************
    *  env_load()
    *
    *  Load the environment from the NVRAM device.
    *
    *  Input parameters:
    *      nothing
    *
    *  Return value:
    *      0 if ok
    *      else error code
    ********************************************************************* */

int env_load(void)
{
	unsigned int size;
	unsigned char *buffer;
	unsigned char *ptr;
	unsigned char *envval;
	unsigned int reclen;
	unsigned int rectype;
	int offset;
	int flg;
	int retval = -1;
	char valuestr[ENV_MAX_RECORD + 1];

	/*
	 * If in 'safe mode', don't read the environment the first time.
	 */
	if (bolt_startflags & BOLT_INIT_SAFE) {
		bolt_startflags &= ~BOLT_INIT_SAFE;
		return 0;
	}

	flg = nvram_open();
	if (flg < 0)
		return flg;

	size = nvram_getsize();
	buffer = KMALLOC(size, 0);
	if (buffer == NULL)
		return BOLT_ERR_NOMEM;

	ptr = buffer;
	offset = 0;
	/* Read the record type and length */
	if (nvram_read(ptr, offset, 1) != 1) {
		retval = BOLT_ERR_IOERR;
		goto error;
	}
	while ((*ptr != ENV_TLV_TYPE_END) && (size > 1)) {

		/* Adjust pointer for TLV type */
		rectype = *(ptr);
		offset++;
		size--;

		/*
		 * Read the length.  It can be either 1 or 2 bytes
		 * depending on the code
		 */
		if (rectype & ENV_LENGTH_8BITS) {
			/* Read the record type and length - 8 bits */
			if (nvram_read(ptr, offset, 1) != 1) {
				retval = BOLT_ERR_IOERR;
				goto error;
			}
			reclen = *(ptr);
			size--;
			offset++;
		} else {
			/* Read the record type and length - 16 bits,
			MSB first */
			if (nvram_read(ptr, offset, 2) != 2) {
				retval = BOLT_ERR_IOERR;
				goto error;
			}
			reclen =
			    (((unsigned int)*(ptr)) << 8) +
			    (unsigned int)*(ptr + 1);
			size -= 2;
			offset += 2;
		}
		if (reclen > size)
			break;	/* should not happen, bad NVRAM */

		switch (rectype) {
		case ENV_TLV_TYPE_ENV:
			/* Read the TLV data */
			if ((unsigned int)nvram_read(ptr, offset, reclen) !=
					reclen)
				goto error;
			flg = *ptr++;
			envval =
			    (unsigned char *)strnchr((char *)ptr, '=',
						     (reclen - 1));
			if (envval) {
				*envval++ = '\0';
				memcpy(valuestr, envval,
				       (reclen - 1) - (envval - ptr));
				valuestr[(reclen - 1) - (envval - ptr)] = '\0';
				env_setenv((char *)ptr, valuestr, flg);
			}
			break;

		default:
			/* Unknown TLV type, skip it. */
			break;
		}

		/*
		 * Advance to next TLV
		 */

		size -= (int)reclen;
		offset += reclen;

		/* Read the next record type */
		ptr = buffer;
		if (nvram_read(ptr, offset, 1) != 1)
			goto error;
	}

	retval = 0;		/* success! */

error:
	KFREE(buffer);
	nvram_close();

	return retval;

}

/*  *********************************************************************
    *  env_save()
    *
    *  Write the environment to the NVRAM device.
    *
    *  Input parameters:
    *      nothing
    *
    *  Return value:
    *      0 if ok, else error code
    ********************************************************************* */

int env_save(void)
{
	int size;
	unsigned char *buffer;
	unsigned char *buffer_end;
	unsigned char *ptr;
	queue_t *qb;
	struct bolt_envvar_s *env;
	int namelen;
	int valuelen;
	int reclen;
	int flg;

	flg = nvram_open();
	if (flg < 0)
		return flg;

	nvram_erase();

	size = nvram_getsize();
	buffer = KMALLOC(size, 0);

	if (buffer == NULL)
		return BOLT_ERR_NOMEM;

	buffer_end = buffer + size;

	ptr = buffer;

	for (qb = env_envvars.q_next; qb != &env_envvars; qb = qb->q_next) {
		env = (struct bolt_envvar_s *) qb;

		if (env->flags & (ENV_FLG_BUILTIN))
			continue;

		namelen = strlen(env->name);
		valuelen = strlen(env->value);

		if ((ptr + 2 + namelen + valuelen + 1 + 1 + 1) > buffer_end)
			break;

		reclen = (namelen + valuelen + 1 + 1);	/* TLV record length */
		if (reclen > ENV_MAX_RECORD) {
			err_msg("env save: %s is too long, not writing to flash",
				env->name);
			env->flags |= ENV_FLG_BUILTIN;
			continue;
		}

		*ptr++ = ENV_TLV_TYPE_ENV;	/* TLV record type */
		*ptr++ = reclen;

		*ptr++ = (unsigned char)env->flags;
		memcpy(ptr, env->name, namelen);	/* TLV record data */
		ptr += namelen;
		*ptr++ = '=';
		memcpy(ptr, env->value, valuelen);
		ptr += valuelen;

	}

	*ptr++ = ENV_TLV_TYPE_END;

	size = nvram_write(buffer, 0, ptr - buffer);

	KFREE(buffer);

	nvram_close();

	return (size == (ptr - buffer)) ? 0 : BOLT_ERR_IOERR;
}
