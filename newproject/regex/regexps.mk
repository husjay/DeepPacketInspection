
regexps.dll: dlldata.obj regex_p.obj regex_i.obj
	link /dll /out:regexps.dll /def:regexps.def /entry:DllMain dlldata.obj regex_p.obj regex_i.obj \
		kernel32.lib rpcndr.lib rpcns4.lib rpcrt4.lib oleaut32.lib uuid.lib \

.c.obj:
	cl /c /Ox /DWIN32 /D_WIN32_WINNT=0x0400 /DREGISTER_PROXY_DLL \
		$<

clean:
	@del regexps.dll
	@del regexps.lib
	@del regexps.exp
	@del dlldata.obj
	@del regex_p.obj
	@del regex_i.obj
