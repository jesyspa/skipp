Help("""
'scons debug' for making the debug build
'scons test' for building tests
'scons release' for making the release build
'scons reltest' for building tests in release mode
'scons doc' for generating documentation
""")

########  FLAGS  ########

flags = {
   'cpp': ["-Wall", "-Wextra", "-pedantic", "-Werror", "-std=c++1y"],
   'debug': ["-O0", "-ggdb", "-D_GLIBCXX_DEBUG"],
   'release': ["-O2", "-DNDEBUG"],
   'skipp': "-I#include/ -L#lib",
   'test': ["-I#include/", "-lboost_unit_test_framework"],
}

Export('flags')

######  END FLAGS  ######

##### ENVIRONMENTS  #####

base_env = Environment(CXX="g++", CPPFLAGS=flags['cpp'])

dbg_skipp_env = base_env.Clone()
dbg_skipp_env.MergeFlags([flags['debug'], flags['skipp']])

rel_skipp_env = base_env.Clone()
rel_skipp_env.MergeFlags([flags['release'], flags['skipp']])

Export('base_env')

#### END ENVIRONMENTS ###

dbg_exports = {
   'env': dbg_skipp_env,
   'prefix': 'dbg',
}

rel_exports = {
   'env': rel_skipp_env,
   'prefix': '',
} 

SConscript("src/SConscript", exports=dbg_exports, variant_dir='.dbgbuild')
SConscript("src/SConscript", exports=rel_exports, variant_dir='.relbuild')
Command('doc/.create', 'Doxyfile', 'doxygen')

Alias('dbg', '#bin/dbgskipp')
Alias('test', '#bin/dbgtest')
Alias('release', '#bin/skipp')
Alias('reltest', '#bin/test')
Alias('doc', 'doc/.create')

Default('test')

