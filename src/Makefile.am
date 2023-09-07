AM_CPPFLAGS = \
	$(LIBDOVECOT_INCLUDE) \
	$(LIBDOVECOT_PUSH_NOTIFICATION_INCLUDE)

push_notification_plugin_chronos_sources = \
	push-notification-plugin-chronos.c \
	push-notification-driver-chronos.c

noinst_HEADERS = \
	push-notification-plugin-chronos.h

lib22_push_notification_chronos_plugin_la_LDFLAGS = -module -avoid-version

dovecot_module_LTLIBRARIES = \
	lib22_push_notification_chronos_plugin.la

lib22_push_notification_chronos_plugin_la_LIBADD = \
	$(LIBDOVECOT)

lib22_push_notification_chronos_plugin_la_DEPENDENCIES = \
	$(LIBDOVECOT_DEPS)

lib22_push_notification_chronos_plugin_la_SOURCES = \
	$(push_notification_plugin_chronos_sources)

settingsdir = $(dovecot_moduledir)/settings
settings_LTLIBRARIES = $(set_lib)

headers =

pkginc_libdir=$(dovecot_pkgincludedir)
pkginc_lib_HEADERS=$(headers)
