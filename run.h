#ifndef __IB__RUN__H__
#define __IB__RUN__H__

#include <cstdint>
#include <cstring>
#include <fstream>
#include <string>
#include <vector>

#include <sys/socket.h>
#include <sys/unistd.h>
#include <sys/wait.h>

#include "ib/logger.h"
#include "ib/marshalled.h"
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
		vector<string> pipeline;
		_pipes.push_back(nullptr);
		_pipes.back().reset(new PipePair());

		Tokenizer::split_mind_quote(cmd, "|", &pipeline);
		for (auto& x: pipeline) {
			connect(x);
		}
		write_input(input);
	}

	void connect(const string& cmd) {
		_argvs.push_back(vector<string>());
		_pipes.push_back(nullptr);
		_pipes.back().reset(new PipePair());
		Tokenizer::split_mind_quote(cmd, " ", &_argvs.back());
	}

protected:
	pid_t execute(size_t pos) {
		const char& c= _argvs[pos][0][0];
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
		Logger::info("(ib::run) calling: %", _argvs[pos]);

		pid_t pid = fork();
		if (pid == -1) {
			throw string("fork(): failed");
		}
		if (pid == 0) {
			char** argv = get_c_args(pos);
			_pipes[pos]->set_read();
			dup2(_pipes[pos]->read_end, STDIN_FILENO);
			// TODO: decide what to do with STDERR_FILENO
			_pipes[pos + 1]->set_write();
			dup2(_pipes[pos + 1]->write_end, STDOUT_FILENO);
			execv(argv[0], (char * const *) argv);
			if (errno == 2) {
				Logger::error("execv failed. did you give a full path to executable?");
			} else {
				Logger::error("execv failed: %", errno);
			}
			_pipes[pos]->close();
			_pipes[pos + 1]->close();
			char** p = argv;
			while (!*p++) free(*p);
			free(argv);
			exit(-1);
		}
		_pipes[pos]->close();
		return pid;
	}

public:
	/* TODO: have a start / wait threaded one */
	void operator()() {
		assert(!_started);
		_started = true;
		size_t pos = 0;
		while (pos != _argvs.size()) {
			_pids.push_back(execute(pos));
			++pos;
		}
		_pipes.back()->set_read();
	}

	int result() const {
		return _status;
	}

	int redirect(const string& filename) {
		ofstream fout(filename);
		const size_t SIZE = 4096;
		assert(_pipes.back()->read_end);
		int r = 0;
		char buf[SIZE];
		do {
			r = ::read(_pipes.back()->read_end, buf, SIZE);
			fout.write(buf, r);
		} while (r > 0);
		if (r < 0) {
			Logger::error("read failed: % %", r, errno);
			return -1;
		}
		return 0;
	}

	string read() {
		const size_t SIZE = 4096;
		stringstream ss;
		assert(_pipes.size());
		assert(_pids.size());
		assert(_pipes.back()->read_end);
		char buf[SIZE];
		size_t pid_pos = 0;
		while (true) {
			int r = ::read(_pipes.back()->read_end, buf, SIZE);
			if (r == 0) {
				waitpid(_pids[pid_pos++], &_status, 0);
				Logger::info("(run) pid % finished %",
					     _pids[pid_pos], _status);
				if (pid_pos == _pids.size()) break;
			}
			if (r < 0) {
				Logger::error("read failed % %", r, errno);
				return "";
			}
			ss.write(buf, r);
		}
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
	bool _started;
};

}  // namespace ib

#endif  // __IB__RUN__H__
