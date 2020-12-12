PREFIX?=/usr/local
MANPREFIX?=${PREFIX}/share/man
STRIP?=strip

default: map

map: map.c
	$(CC) -Wall -Os -o map map.c

clean:
	@echo Cleaning up
	@rm -f map

install: map
	@echo Stripping executable
	@${STRIP} map
	@echo Installing executable to ${PREFIX}/bin
	@mkdir -p ${PREFIX}/bin
	@cp -f map ${PREFIX}/bin/map
	@chmod 755 ${PREFIX}/bin/map

	@echo installing manual pages to ${MANPREFIX}/man1
	@mkdir -p ${MANPREFIX}/man1
	@cp -f map.1 ${MANPREFIX}/man1/map.1
	@chmod 644 ${MANPREFIX}/man1/map.1

uninstall:
	@echo Removing executable from ${PREFIX}/bin
	@rm -f ${PREFIX}/bin/map
	@echo Removing manual pages from ${MANPREFIX}/man1
	@rm -f ${MANPREFIX}/man1/map.1

test:
	@bash test/tests.sh

.PHONY: clean install uninstall test
