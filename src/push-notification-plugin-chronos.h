#ifndef PUSH_NOTIFICATION_PLUGIN_CHRONOS_H
#define PUSH_NOTIFICATION_PLUGIN_CHRONOS_H

extern const char *push_notification_chronos_plugin_dependencies[];

struct module;

void push_notification_chronos_plugin_init(struct module *module);
void push_notification_chronos_plugin_deinit(void);

#endif /* PUSH_NOTIFICATION_PLUGIN_CHRONOS_H */
