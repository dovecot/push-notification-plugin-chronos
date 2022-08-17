/* Copyright (c) 2022 Open-Xchange Software GmbH, see the included COPYING file */

#include "lib.h"

#include "push-notification-drivers.h"
#include "chronos-push-notification-plugin.h"

/* Plugin interface */

const char *chronos_push_notification_plugin_version = DOVECOT_ABI_VERSION;
const char *chronos_push_notification_plugin_dependencies[] =
	{ "push_notification", NULL };

extern struct push_notification_driver chronos_push_notification_driver;

void chronos_push_notification_plugin_init(struct module *module ATTR_UNUSED) {
	push_notification_driver_register(&chronos_push_notification_driver);
}

void chronos_push_notification_plugin_deinit(void) {
	push_notification_driver_unregister(&chronos_push_notification_driver);
}
