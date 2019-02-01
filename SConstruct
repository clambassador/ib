import os
for i in range(0, 10):
    print ""
common = Split("""config.cc
	          logger.cc
		  sensible_time.cc
		  fileutil.cc
	       """)
tools = dict()
tools['xor.cc'] = 'xor'
tools['slidemask.cc'] = 'slidemask'
tools['csv_join.cc'] = 'csv_join'
tools['hex_unescape.cc'] = 'hex_unescape'
tools['json_to_csv.cc'] = 'json_to_csv'
tools['extract_all.cc'] = 'extract_all'
tools['zcat.cc'] = 'zcat'
tests = dict()
tests['test_limiter.cc'] = 'test_limiter'
tests['test_marshalled.cc'] = 'test_marshalled'
tests['test_config.cc'] = 'test_config'
tests['test_run.cc'] = 'test_run'
tests['test_run_abort.cc'] = 'test_run_abort'
tests['test_tokenizer.cc'] = 'test_tokenizer'
tests['test_formatting.cc'] = 'test_formatting'
tests['test_exceptional.cc'] = 'test_exceptional'
tests['test_csv_map.cc'] = 'test_csv_map'
tests['test_csv_table.cc'] = 'test_csv_table'
tests['test_csv_stream.cc'] = 'test_csv_stream'
tests['test_wait_queue.cc'] = 'test_wait_queue'
tests['test_csv_join.cc'] = 'test_csv_join'
tests['test_containers.cc'] = 'test_containers'
tests['test_set.cc'] = 'test_set'
tests['test_base64.cc'] = 'test_base64'
tests['add_csv.cc'] = 'add_csv'
tests['csv_to_xml.cc'] = 'csv_to_xml'

libs = Split("""z
	     """)
env = Environment(CXX="ccache clang++ -I.. -pthread", 		  CPPFLAGS="-D_FILE_OFFSET_BITS=64 -Wall -g --std=c++11", LIBS=libs, CPPPATH="..")
env['ENV']['TERM'] = 'xterm'

Decider('MD5')
for i in tests:
	env.Program(source = ['tests/' + i] + common, target = tests[i])
for i in tools:
	env.Program(source = ['tools/' + i] + common, target = tools[i])

env.Library('ib', common)
