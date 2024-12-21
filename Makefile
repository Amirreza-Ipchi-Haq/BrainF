CC=gcc
COMMITMESSAGE=""
EDIT=vi
ICON=icon.ico
SRC=main.c
STRIP=strip
WINSTRIP=i686-w64-mingw32-strip
TARGET=BrainF
WINCC=i686-w64-mingw32-gcc
WINDRES=i686-w64-mingw32-windres
commit:
	git add .
	if test -n "${COMMITMESSAGE}";then\
		git commit -m "${COMMITMESSAGE}";\
	else\
		git commit;\
	fi
compileLinux:
	${CC} ${SRC} -o ${TARGET}_Linux -m32 -static -Wall -Wextra -Os
	${STRIP} -s ${TARGET}_Linux
compileOSX:
	${CC} ${SRC} -o ${TARGET}_OSX_x64 -Wall -Wextra -Os --target=x86_64-apple-darwin
	${CC} ${SRC} -o ${TARGET}_OSX_arm64 -Wall -Wextra -Os --target=arm64-apple-darwin
	lipo -create -output ${TARGET}_OSX ${TARGET}_OSX_x64 ${TARGET}_OSX_arm64
	rm ${TARGET}_OSX_x64 ${TARGET}_OSX_arm64
	${STRIP} ${TARGET}_OSX
compileWindows:
	echo 'MAINICON ICON "${ICON}"'>resource.rc
	${WINDRES} -O coff -o resource.res resource.rc
	${WINCC} ${SRC} -o ${TARGET}_Windows.exe resource.res -static -Wall -Wextra -Os
	rm resource.rc resource.res
	${WINSTRIP} -s ${TARGET}_Windows.exe
edit:
	${EDIT} ${SRC}
fetch:
	git fetch
help:
	echo "Makefile commands:\n\tcommit [COMMITMESSAGE=<message>]  Commit to the repository using\n\t                                  <message> as the commit message (Leave\n\t                                  empty to enter description as well)\n\tcompileLinux                      Compile for Linux\n\tcompileOSX                        Compile for OSX\n\tcompileWindows                    Compile for Windows\n\tedit [EDIT=<editor>]              Edit the source code (using Vim by\n\t                                  default)\n\tfetch                             Fetch updates from the repository\n\thelp                              Show help\n\tpush                              Push updates to the repository\n\trun                               Run on Linux"
push:
	git push
run:
	./${TARGET}_Linux
