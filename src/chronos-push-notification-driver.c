/* Copyright (c) 2022 Open-Xchange Software GmbH, see the included COPYING file */

#include "lib.h"
#include "push-notification-drivers.h"
#include "push-notification-plugin.h"

struct chronos_push_notification_driver_config {
	struct event *event;
};

static int
chronos_push_notification_driver_init(
	struct push_notification_driver_config *config ATTR_UNUSED,
	struct mail_user *user, pool_t pool,
	void **context, const char **error_r ATTR_UNUSED)
{
	struct chronos_push_notification_driver_config *dconfig;

	dconfig = p_new(pool, struct chronos_push_notification_driver_config, 1);
	dconfig->event = event_create(user->event);
	event_add_category(dconfig->event, push_notification_get_event_category());
	event_set_append_log_prefix(dconfig->event, "push-notification-chronos: ");

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
}

static void
chronos_push_notification_driver_cleanup(void)
{
	return;
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
