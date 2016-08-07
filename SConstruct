import os
for i in range(0, 10):
    print ""
common = Split("""config.cc
	          logger.cc
		  sensible_time.cc
	       """)
tests = dict()
tests['test_limiter.cc'] = 'test_limiter'
tests['test_marshalled.cc'] = 'test_marshalled'
tests['test_config.cc'] = 'test_config'

libs = Split("""
	     """)
env = Environment(CXX="ccache clang++ -I..", CPPFLAGS="-D_FILE_OFFSET_BITS=64 -Wall -g --std=c++11 -pthread", LIBS=libs, CPPPATH="..")
env['ENV']['TERM'] = 'xterm'

Decider('MD5')
for i in tests:
	env.Program(source = ['tests/' + i] + common, target = tests[i])

env.Library('ib', common)

