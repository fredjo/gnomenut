LANGUAGES=cs da de en_US es fr hu it ja ko nb pl pt_BR nl ru sv sk tr wa zh_CN
MOFILES=cs.mo da.mo de.mo en_US.mo es.mo fr.mo hu.mo it.mo ja.mo ko.mo nb.mo pl.mo pt_BR.mo nl.mo ru.mo sv.mo sk.mo tr.mo wa.mo zh_CN.mo
INSTALL = /usr/bin/install -c
prefix=/usr/local
LOCALEDIR=${prefix}/share/locale
PACKAGE=mplayerplug-in

.SUFFIXES:
.SUFFIXES: .po .mo .pot
.PHONY: all install uninstall clean distclean $(LANGUAGES)

.po.mo:
	msgfmt -o $@ $<

all: $(MOFILES)

install: $(MOFILES)
	for lang in $(LANGUAGES); do \
	  $(INSTALL) -d -m0755 $(DESTDIR)$(LOCALEDIR)/$$lang/LC_MESSAGES; \
	  if test -r $$lang.mo; then \
	    $(INSTALL) -m644 $$lang.mo $(DESTDIR)$(LOCALEDIR)/$$lang/LC_MESSAGES/$(PACKAGE).mo; \
	  fi; \
	done


uninstall:
	for lang in $(LANGUAGES); do \
	  rm -f $(DESTDIR)$(LOCALEDIR)/$$lang/LC_MESSAGES/$(PACKAGE).mo; \
	done

clean:
	rm -f core core.* *.old.po *.mo *.pot sjiscorr

distclean: clean

$(PACKAGE).pot: ../Source/*.cpp
	xgettext --default-domain=$(PACKAGE) --add-comments --keyword=_ --keyword=N_ ../Source/*.cpp -o $(PACKAGE).pot

$(LANGUAGES):
	@$(MAKE) $(PACKAGE).pot
	if test ! -f $@.po.orig; then cp $@.po $@.po.orig; fi
	mv $@.po $@.po.old
	if msgmerge $@.po.old $(PACKAGE).pot -o $@.po; then \
	    rm -f $@.po.old; \
	else \
	    echo "msgmerge for $@.po failed!"; mv $@.po.old $@.po; \
	fi
