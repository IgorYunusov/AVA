#include <string.h>

char *urihttp( char *buf, int N, const char *fmt ) {
	const char *proto = strstr( fmt, "://" );
	_snprintf( buf, N, "%.*s", proto ? proto - fmt + 3 : 0, fmt );
	return buf;
}
char *uriaddr( char *buf, int N, const char *fmt ) {
	const char *addr = &fmt[strlen(urihttp(buf, N, fmt))];
	const char *colon = strstr( addr, ":" );
	_snprintf( buf, N, "%.*s", colon ? colon - addr : strlen(addr), addr );
	return buf;
}
char *uriport( char *buf, int N, const char *fmt ) {
	const char *addr = &fmt[strlen(urihttp(buf, N, fmt))];
	const char *colon = strstr( addr, ":" );
	_snprintf( buf, N, "%s", colon && colon[1] ? &colon[1] : "" );
	return buf;
}

#ifdef DEMO
#include <stdio.h>

int main() {
	char buf[1024];

	puts("---");
	puts( urihttp(buf, 1024, "lala") );
	puts( uriaddr(buf, 1024, "lala") );
	puts( uriport(buf, 1024, "lala") );

	puts("---");
	puts( urihttp(buf, 1024, "file://lala") );
	puts( uriaddr(buf, 1024, "file://lala") );
	puts( uriport(buf, 1024, "file://lala") );

	puts("---");
	puts( urihttp(buf, 1024, "unix://lala") );
	puts( uriaddr(buf, 1024, "unix://lala") );
	puts( uriport(buf, 1024, "unix://lala") );

	puts("---");
	puts( urihttp(buf, 1024, "tcp://lala") );
	puts( uriaddr(buf, 1024, "tcp://lala") );
	puts( uriport(buf, 1024, "tcp://lala") );

	puts("---");
	puts( urihttp(buf, 1024, "lala:3200") );
	puts( uriaddr(buf, 1024, "lala:3200") );
	puts( uriport(buf, 1024, "lala:3200") );

	puts("---");
	puts( urihttp(buf, 1024, "file://lala:3201") );
	puts( uriaddr(buf, 1024, "file://lala:3201") );
	puts( uriport(buf, 1024, "file://lala:3201") );

	puts("---");
	puts( urihttp(buf, 1024, "unix://lala:3202") );
	puts( uriaddr(buf, 1024, "unix://lala:3202") );
	puts( uriport(buf, 1024, "unix://lala:3202") );

	puts("---");
	puts( urihttp(buf, 1024, "tcp://lala:3203") );
	puts( uriaddr(buf, 1024, "tcp://lala:3203") );
	puts( uriport(buf, 1024, "tcp://lala:3203") );

	puts("---");
	puts( urihttp(buf, 1024, "tcp://resource:3204") );
	puts( uriaddr(buf, 1024, "tcp://resource:3204") );
	puts( uriport(buf, 1024, "tcp://resource:3204") );
}
#endif
