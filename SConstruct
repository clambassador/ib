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
tests['test_run.cc'] = 'test_run'
tests['test_tokenizer.cc'] = 'test_tokenizer'
tests['test_formatting.cc'] = 'test_formatting'
tests['test_exceptional.cc'] = 'test_exceptional'
tests['test_csv_table.cc'] = 'test_csv_table'
tests['test_wait_queue.cc'] = 'test_wait_queue'
tests['add_csv.cc'] = 'add_csv'

libs = Split("""
	     """)
env = Environment(CXX="ccache clang++ -I.. -pthread", CPPFLAGS="-D_FILE_OFFSET_BITS=64 -Wall -g --std=c++11", LIBS=libs, CPPPATH="..")
env['ENV']['TERM'] = 'xterm'

Decider('MD5')
for i in tests:
	env.Program(source = ['tests/' + i] + common, target = tests[i])

env.Library('ib', common)
