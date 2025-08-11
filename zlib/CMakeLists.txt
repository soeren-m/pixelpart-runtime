target_sources(pixelpart-runtime PRIVATE
	adler32.c
	compress.c
	crc32.c
	deflate.c
	gzclose.c
	gzlib.c
	gzread.c
	gzwrite.c
	infback.c
	inffast.c
	inflate.c
	inftrees.c
	trees.c
	uncompr.c
	zutil.c)

include(CheckIncludeFile)

check_include_file(stdarg.h HAVE_STDARG_H)
check_include_file(unistd.h HAVE_UNISTD_H)

if(HAVE_STDARG_H)
	add_compile_definitions(HAVE_STDARG_H)
endif()
if(HAVE_UNISTD_H)
	add_compile_definitions(HAVE_UNISTD_H)
endif()
