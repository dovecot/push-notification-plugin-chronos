SUBDIRS = src

ACLOCAL_AMFLAGS = -I m4

EXTRA_DIST = \
	LICENSE \
	README.md \
	version.txt

version.txt:
	$(AM_V_GEN)$(top_srcdir)/build-aux/git-version-gen > $@
