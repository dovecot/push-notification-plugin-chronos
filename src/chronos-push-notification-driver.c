/* Copyright (c) 2022 Open-Xchange Software GmbH, see the included COPYING file */

#include "lib.h"
#include "hash.h"
#include "http-client.h"
#include "http-url.h"
#include "iostream-ssl.h"
#include "push-notification-drivers.h"
#include "push-notification-plugin.h"
#include "settings-parser.h"
#include "str.h"

/* Default values. */
#define DEFAULT_MSG_MAX_SIZE 512000
#define DEFAULT_RETRY_COUNT 1
#define DEFAULT_TIMEOUT_MSECS 2000

struct chronos_push_notification_driver_global {
	struct http_client *http_client;
	int refcount;
};

static struct chronos_push_notification_driver_global *chronos_global = NULL;

struct chronos_push_notification_driver_config {
	struct event *event;
	struct http_url *http_url;
	unsigned int http_max_retries;
	unsigned int http_timeout_msecs;
	uoff_t msg_max_size;
};

static void
chronos_push_notification_driver_init_global(
	struct chronos_push_notification_driver_config *config,
	struct mail_user *user)
{
	chronos_global = i_new(struct chronos_push_notification_driver_global, 1);

	struct http_client_settings http_set;
	i_zero(&http_set);
	/* This is going to use the first user's settings, but these
	   are unlikely to change between users so it shouldn't matter
	   much. */
	http_set.debug = user->mail_debug;
	http_set.max_attempts = config->http_max_retries + 1;
	http_set.request_timeout_msecs = config->http_timeout_msecs;
	http_set.event_parent = user->event;

	struct ssl_iostream_settings ssl_set;
	mail_user_init_ssl_client_settings(user, &ssl_set);
	http_set.ssl = &ssl_set;

	chronos_global->http_client = http_client_init(&http_set);
	chronos_global->refcount = 1;
}

static void
chronos_push_notification_driver_global_unref(void)
{
	if (chronos_global != NULL) {
		if (chronos_global->http_client != NULL)
			http_client_wait(chronos_global->http_client);
		i_assert(chronos_global->refcount > 0);
		--chronos_global->refcount;

		if (chronos_global->refcount <= 0) {
			if (chronos_global->http_client != NULL)
				http_client_deinit(&chronos_global->http_client);
			i_free(chronos_global);
		}
	}
}

static int
chronos_push_notification_driver_init(
	struct push_notification_driver_config *config,
	struct mail_user *user, pool_t pool, void **context,
	const char **error_r)
{
	struct chronos_push_notification_driver_config *dconfig;
	const char *config_item, *error;

	dconfig = p_new(pool, struct chronos_push_notification_driver_config, 1);
	dconfig->event = event_create(user->event);
	event_add_category(dconfig->event, push_notification_get_event_category());
	event_set_append_log_prefix(dconfig->event, "push-notification-chronos: ");

	config_item = hash_table_lookup(config->config, (const char *)"url");
	if (config_item == NULL) {
		e_error(dconfig->event, "URL config missing");
		event_unref(&dconfig->event);
		*error_r = "Driver requires the url parameter";
		return -1;
	}

	if (http_url_parse(config_item, NULL, HTTP_URL_ALLOW_USERINFO_PART,
			   pool, &dconfig->http_url, &error) < 0) {
		event_unref(&dconfig->event);
		*error_r = t_strdup_printf(
			"Failed to parse Chronos Push Notification URL '%s': %s",
			config_item, error);
		return -1;
	}

	config_item = hash_table_lookup(config->config, (const char *)"max_retries");
	if ((config_item == NULL) ||
	    (str_to_uint(config_item, &dconfig->http_max_retries) < 0)) {
		if (config_item == NULL) {
			config_item = "(unset)";
		}
		e_warning(dconfig->event,
			  "Unable to parse setting for \"max_retries\" value: %s. "
			  "Using default value: 1.", config_item);
		dconfig->http_max_retries = DEFAULT_RETRY_COUNT;
	}

	config_item = hash_table_lookup(config->config, (const char *)"timeout");
	if ((config_item == NULL) ||
	    (settings_get_time_msecs(config_item, &dconfig->http_timeout_msecs, &error) < 0)) {
		if (config_item == NULL) {
			config_item = "(unset)";
			error = "";
		}
		e_warning(dconfig->event,
			  "Unable to parse setting for \"timeout\" value: %s. "
			  "Using default value: %dms. %s",
			  config_item, DEFAULT_TIMEOUT_MSECS, error);
		dconfig->http_timeout_msecs = DEFAULT_TIMEOUT_MSECS;
	}

	config_item = hash_table_lookup(config->config, (const char *)"msg_max_size");
	if ((config_item == NULL) ||
	    (settings_get_size(config_item, &dconfig->msg_max_size, &error) < 0)) {
		if (config_item == NULL) {
			config_item = "(unset)";
			error = "";
		}
		e_warning(dconfig->event,
			  "Unable to parse setting for \"msg_max_size\" value: %s. "
			  "Using default value: %dB. %s",
			  config_item, DEFAULT_MSG_MAX_SIZE, error);
		dconfig->msg_max_size = DEFAULT_MSG_MAX_SIZE;
	}

	e_debug(dconfig->event,
		"Using config: max retries = %u, timeout = %ums, "
		"msg max size = %"PRIuUOFF_T"B",
		dconfig->http_max_retries, dconfig->http_timeout_msecs,
		dconfig->msg_max_size);

	if (chronos_global == NULL) {
		chronos_push_notification_driver_init_global(dconfig, user);
	} else {
		i_assert(chronos_global->refcount > 0);
		++chronos_global->refcount;
	}

	*context = dconfig;

	return 0;
}

static bool
chronos_push_notification_driver_begin_txn(
	struct push_notification_driver_txn *dtxn ATTR_UNUSED)
{
	return FALSE;
}

static void
chronos_push_notification_driver_process_msg(
	struct push_notification_driver_txn *dtxn ATTR_UNUSED,
	struct push_notification_txn_msg *msg ATTR_UNUSED)
{
	return;
}

static void
chronos_push_notification_driver_deinit(
	struct push_notification_driver_user *duser)
{
	struct chronos_push_notification_driver_config *dconfig = duser->context;
	event_unref(&dconfig->event);

	chronos_push_notification_driver_global_unref();
}

static void
chronos_push_notification_driver_cleanup(void)
{
	chronos_push_notification_driver_global_unref();
}

/* Driver definition */

extern struct push_notification_driver chronos_push_notification_driver;

struct push_notification_driver chronos_push_notification_driver = {
	.name = "chronos",
	.v = {
		.init = chronos_push_notification_driver_init,
		.begin_txn = chronos_push_notification_driver_begin_txn,
		.process_msg = chronos_push_notification_driver_process_msg,
		.deinit = chronos_push_notification_driver_deinit,
		.cleanup = chronos_push_notification_driver_cleanup,
	},
};
