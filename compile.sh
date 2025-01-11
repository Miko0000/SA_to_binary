convert icon.png -resize 48x48 icon-48x48.png

glib-compile-resources \
	--generate-source \
	--manual-register \
	--c-name gr_kw \
	resources.xml

glib-compile-resources \
	--generate-header \
	--manual-register \
	--c-name gr_kw \
	resources.xml

TARGET_STR=""
ARCH="$(gcc -dumpmachine)"
if test -n "$TARGET"; then
	TARGET_STR="-target $TARGET"
	ARCH=$TARGET
fi

CPU_STR=""
if test -n "$CPU"; then
	CPU_STR="-$CPU"
fi

clang++ $( pkg-config --cflags gtk4 ) -o bin/sa-to-binary_$ARCH \
	main.cpp $( pkg-config --libs gtk4 ) $TARGET_STR $CPU_STR
