#ifndef __IB__RUN__H__
#define __IB__RUN__H__

#include <cstdint>
#include <cstring>
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
	Run() : Run("", vector<string>()) {}
	Run(const string& cmd) : Run(cmd, vector<string>()) {}
	Run(const string& cmd, const vector<string>& input) {
		vector<string> pipeline;
		_pipes.push_back(nullptr);
		_pipes.back().reset(new PipePair());

		Tokenizer::split_mind_quote(cmd, "|", &pipeline);
		Logger::info("pipeline %", pipeline);
		for (auto& x: pipeline) {
			connect(x);
		}
		prepare_input(input);
	}

	void connect(const string& cmd) {
		_argvs.push_back(vector<string>());
		_pipes.push_back(nullptr);
		_pipes.back().reset(new PipePair());
		Tokenizer::split_mind_quote(cmd, " ", &_argvs.back());
	}

protected:
	pid_t execute(size_t pos) {
		Logger::info("about to call %", _argvs[pos]);
		pid_t pid = fork();
		if (pid == -1) {
			throw "fork(): failed";
		}
		if (pid == 0) {
			char** argv = get_c_args(pos);
			int i = 0;
			while (argv[i] != nullptr) {
				cout << argv[i] << endl;
				++i;
			}
			Logger::info("done!");
			_pipes[pos]->set_read();
			dup2(_pipes[pos]->read_end, STDIN_FILENO);
			// TODO: decide what to do with STDERR_FILENO
			_pipes[pos + 1]->set_write();
			dup2(_pipes[pos + 1]->write_end, STDOUT_FILENO);
			execv(argv[0], (char * const *) argv);
			Logger::error("execv failed %", errno);
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
		size_t pos = 0;
		while (pos != _argvs.size()) {
			pid_t pid = execute(pos);
			waitpid(pid, &_status, 0);
			_pipes[pos]->close();
			++pos;
		}
		_pipes.back()->set_read();
	}

	string read() {
		const size_t SIZE = 4096;
		stringstream ss;
		assert(_pipes.back()->read_end);
		char buf[SIZE];
		while (true) {
			int r = ::read(_pipes.back()->read_end, buf, SIZE);
			if (r == 0) break;
			if (r < 0) {
				Logger::error("read failed % %", r, errno);
				return "";
			}
			ss << string(buf, r);
		}
		return ss.str();
	}

protected:
	void prepare_input(const vector<string>& input) {
		write(_pipes.front().get(), Marshalled(input).str());
		_pipes.front()->close();
	}

	void write(PipePair* pipe, const string& data) {
		int r = ::write(pipe->write_end, data.c_str(), data.length());
		if (r != data.length()) throw "write(): failed.";
	}

	char** get_c_args(int pos) {
		char** retval = new char*[_argvs[pos].size() + 1];

		for (size_t i = 0; i < _argvs[pos].size(); ++i) {
			retval[i] = new char[_argvs[pos][i].length() + 1];
			strncpy(retval[i], _argvs[pos][i].c_str(),
			       _argvs[pos][i].length());
			retval[i][_argvs[pos][i].length()] = 0;
			Logger::info("% %", (void*)retval[i], retval[i]);
		}
		retval[_argvs[pos].size()] = nullptr;
		return retval;
	}

	vector<vector<string>> _argvs;
	vector<unique_ptr<PipePair>> _pipes;
	int _status;
};

}  // namespace ib

#endif  // __IB__RUN__H__
