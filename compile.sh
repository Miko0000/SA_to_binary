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

g++ $( pkg-config --cflags gtk4 ) -o bin/sa-to-binary_$(gcc -dumpmachine) \
	main.cpp $( pkg-config --libs gtk4 )
