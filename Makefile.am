SUBDIRS = src
dist_doc_DATA = README.md

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = blockhash.pc

LIBTOOL_DEPS = @LIBTOOL_DEPS@

libtool: @LIBTOOL_DEPS@
	$(SHELL) ./config.status --recheck

dist-hook:
	rm -rf `find $(distdir) -name \.git`

