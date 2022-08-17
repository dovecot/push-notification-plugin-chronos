/* Copyright (c) 2022 Open-Xchange Software GmbH, see the included COPYING file */

#ifndef CHRONOS_PUSH_NOTIFICATION_PLUGIN_H
#define CHRONOS_PUSH_NOTIFICATION_PLUGIN_H

extern const char *chronos_push_notification_plugin_dependencies[];

struct module;

void chronos_push_notification_plugin_init(struct module *module);
void chronos_push_notification_plugin_deinit(void);

#endif /* CHRONOS_PUSH_NOTIFICATION_PLUGIN_H */
