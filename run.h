#ifndef __IB__RUN__H__
#define __IB__RUN__H__

#include <cstdint>
#include <cstring>
#include <fstream>
#include <functional>
#include <string>
#include <vector>

#include <sys/socket.h>
#include <sys/unistd.h>
#include <sys/wait.h>

#include "ib/logger.h"
#include "ib/marshalled.h"
#include "ib/sensible_time.h"
#include "ib/tokenizer.h"

using namespace std;

namespace ib {

class Run {
	struct PipePair {
		PipePair() { pipe(&read_end); }
		~PipePair() {
			close();
		}
		void close() {
			if (read_end) ::close(read_end);
			if (write_end) ::close(write_end);
		}
		void set_write() {
			::close(read_end);
			read_end = 0;
		}
		void set_read() {
			::close(write_end);
			write_end = 0;
		}

		int read_end;
		int write_end;
		operator int*() {
			return (int*) this;
		}
	};

public:
	Run() : Run("", "") {}
	Run(const string& cmd) : Run(cmd, "") {}
	Run(const string& cmd, const string& input) {
		_started = false;
		_read = false;
		vector<string> pipeline;
		_pipes.push_back(nullptr);
		_pipes.back().reset(new PipePair());

		Tokenizer::split_mind_quote(cmd, "|", &pipeline);
		for (auto& x: pipeline) {
			connect(x);
		}
		write_input(input);
	}

protected:
	void connect(const string& cmd) {
		_argvs.push_back(vector<string>());
		_pipes.push_back(nullptr);
		_pipes.back().reset(new PipePair());
		Tokenizer::split_mind_quote(cmd, " ", &_argvs.back());
		for (size_t i = 0; i < _argvs.back().size(); ++i) {
			_argvs.back()[i] = Tokenizer::collapse_quote(_argvs.back()[i]);
		}
	}

	pid_t execute(size_t pos) {
		const char& c = _argvs[pos][0][0];
		if (c != '/' && c != '~' && c != '.') {
			Run run("/bin/which " + _argvs[pos][0]);
			run();
			string s = run.read();
			if (s.empty()) {
				throw "which(): cannot find \"" +
				      _argvs[pos][0] + "\"";
			}
			_argvs[pos][0] = s.substr(0, s.length() - 1);
		}
		pid_t pid = fork();
		if (pid == -1) {
			throw string("fork(): failed");
		}
		if (pid == 0) {
			char** argv = get_c_args(pos);
			int r = dup2(_pipes[pos]->read_end, STDIN_FILENO);
			close(_pipes[pos]->read_end);
			if (r == -1) {
				Logger::error("dup failed: %", errno);
				exit(-1);
			}
			// TODO: decide what to do with STDERR_FILENO
			dup2(_pipes[pos + 1]->write_end, STDOUT_FILENO);
			close(_pipes[pos + 1]->write_end);
			if (r == -1) {
				Logger::error("dup failed: %", errno);
				exit(-1);
			}
			execv(argv[0], (char * const *) argv);
			if (errno == 2) {
				Logger::error("execv failed for %. did you give a full path to executable?",
					      argv[0]);
			} else {
				Logger::error("execv failed: %", errno);
			}
			_pipes[pos]->close();
			_pipes[pos + 1]->close();
			char** p = argv;
			while (!*p++) delete[](*p);
			delete[] argv;
			exit(-1);
		}
		return pid;

	}

	void abort_run(int when) {
		int now = sensible_time::runtime();
		while (!_done && (now + when > sensible_time::runtime())) {
			usleep(1000);
		}
		if (_done) return;

		for (auto &x : _pids) {
			kill(x, SIGKILL);
		}

	}

public:
	void operator()() {
		assert(!_started);
		_started = true;
		size_t pos = 0;
		_pipes.front()->set_write();
		while (pos != _argvs.size()) {
			_pids.push_back(execute(pos));
			++pos;
		}
		_pipes.back()->set_read();
	}

	int result() {
		if (!_read) read();
		return _status;
	}

	/* TODO: move read and redirect to a single stream based runner */
	int redirect(const string& filename) {
		ofstream fout(filename);
		const size_t SIZE = 4096;
		assert(_pipes.back()->read_end);
		int r = 0;
		size_t pid_pos = 0;
		char buf[SIZE];
		while (true) {
			r = ::read(_pipes.back()->read_end, buf, SIZE);
			if (r == 0) {
				if (pid_pos == _pids.size()) break;
				waitpid(_pids[pid_pos++], &_status, 0);
				continue;
			}
			if (r < 0) {
				Logger::error("read failed: % %", r, errno);
				return -1;
			}
			fout.write(buf, r);
		}
		while (pid_pos < _pids.size()) waitpid(_pids[pid_pos++], &_status, 0);
		return 0;
	}

	string read() {
		return read(1000);
	}

	string read(int runtime) {
		_read = true;
		const size_t SIZE = 4096;
		stringstream ss;
		assert(_pipes.size());
		assert(_pids.size());
		assert(_pipes.back()->read_end);
		char buf[SIZE];
		size_t pid_pos = 0;

		_done = false;
		thread kill_thread(bind(&Run::abort_run, this, runtime));
		int r = 0;
		while (true) {

			r = ::read(_pipes.back()->read_end, buf, SIZE);
			if (r == 0) {
				if (pid_pos == _pids.size()) break;
				waitpid(_pids[pid_pos++], &_status, 0);
				continue;
			}
			if (r < 0) {
				Logger::error("read failed % %", r, errno);
				_done = true;
				kill_thread.join();
				return "";
			}
			ss.write(buf, r);
		}
		while (pid_pos < _pids.size()) waitpid(_pids[pid_pos++], &_status, 0);
		_done = true;
		kill_thread.join();
		return ss.str();
	}

protected:
	void write_input(const string& data) {
		int r = ::write(_pipes.front()->write_end, data.c_str(), data.length());
		if (r != data.length()) throw string("write(): failed.");
	}

	char** get_c_args(int pos) {
		char** retval = new char*[_argvs[pos].size() + 1];

		for (size_t i = 0; i < _argvs[pos].size(); ++i) {
			retval[i] = new char[_argvs[pos][i].length() + 1];
			strncpy(retval[i], _argvs[pos][i].c_str(),
			       _argvs[pos][i].length());
			retval[i][_argvs[pos][i].length()] = 0;
		}
		retval[_argvs[pos].size()] = nullptr;
		return retval;
	}

	vector<vector<string>> _argvs;
	vector<unique_ptr<PipePair>> _pipes;
	vector<pid_t> _pids;
	int _status;
	bool _read;
	bool _done;
	bool _started;
};

}  // namespace ib

#endif  // __IB__RUN__H__
