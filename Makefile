all:
	cc -o secure-notes-exporter/secure-notes-exporter -framework CoreFoundation -framework Security secure-notes-exporter/main.c
