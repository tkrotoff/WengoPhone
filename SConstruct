from wengoscons import wengoenv

env = wengoenv.getGlobalEnvironment()

#Uncomment to compile with MinGW
#env = wengoenv.getGlobalEnvironment(tools=['mingw'])
#env['CC'] = 'gcc'
#env['CXX'] = 'g++'

if env.File('SOptions').exists():
	env.SConscript('SOptions')

#Duplicate = 0 is very important: it tells SCons
#to not duplicate the source code inside the build directory;
#without this it cannot work.
env.SConscript('SConscript', build_dir = env.WengoGetRootBuildDir(), duplicate = 0)
