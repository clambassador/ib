import os
common = Split("""config.cc
	          logger.cc
		  sensible_time.cc
		  fileutil.cc
	       """)
tools = dict()
tools['latexify_images.cc'] = 'latexify_images'
tools['xor_pref.cc'] = 'xor_pref'
tools['xor.cc'] = 'xor'
tools['unmask.cc'] = 'unmask'
tools['slidemask.cc'] = 'slidemask'
tools['csv_join.cc'] = 'csv_join'
tools['hex_unescape.cc'] = 'hex_unescape'
tools['json_packets.cc'] = 'json_packets'
tools['json_to_csv.cc'] = 'json_to_csv'
tools['json_prettyprint.cc'] = 'json_prettyprint'
tools['extract_all.cc'] = 'extract_all'
tools['zcat.cc'] = 'zcat'
tools['alt_base64.cc'] = 'alt_base64'
tools['json.cc'] = 'json'
tools['is_newer.cc'] = 'is_newer'
tools['fsm.cc'] = 'fsm'
tests = dict()
tests['test_find_replace.cc'] = 'test_find_replace'
tests['test_graph.cc'] = 'test_graph'
tests['test_fsm.cc'] = 'test_fsm'
tests['test_down_up_next_parser.cc'] = 'test_down_up_next_parser'
tests['test_limiter.cc'] = 'test_limiter'
tests['test_marshalled.cc'] = 'test_marshalled'
tests['test_config.cc'] = 'test_config'
tests['test_simpleconfig.cc'] = 'test_simpleconfig'
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
tests['test_scanner.cc'] = 'test_scanner'
tests['add_csv.cc'] = 'add_csv'
tests['csv_to_xml.cc'] = 'csv_to_xml'
tests['test_re.cc'] = 'test_re'
tests['test_dupre.cc'] = 'test_dupre'

libs = Split("""z
	     """)
env = Environment(CXX="ccache clang++ -I.. -pthread", 		  CPPFLAGS="-D_FILE_OFFSET_BITS=64 -Wall -g --std=c++11", LIBS=libs, CPPPATH="..")
env['ENV']['TERM'] = 'xterm'
env['ENV']['TARGET'] = 'bin/'

Decider('MD5')
for i in tests:
	env.Program(source = ['tests/' + i] + common, target = tests[i])
for i in tools:
	env.Program(source = ['tools/' + i] + common, target = tools[i])

env.Library('ib', common)
