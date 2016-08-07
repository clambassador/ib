#ifndef __IB__RUN__H__
#define __IB__RUN__H__

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
public:
	Run() : Run("", vector<string>()) {}
	Run(const string& cmd) : Run(cmd, vector<string>()) {}
	Run(const vector<string>& argv) : Run(argv, vector<string>()) {}
	Run(const string& cmd, const vector<string>& input) {
		Tokenizer::split(cmd, " ", &_argv);
		prepare_input(input);
		_socket = 0;
		_error = false;
	}
	Run(const vector<string>& argv, const vector<string>& input) {
		_argv = argv;
		prepare_input(input);
		_socket = 0;
		_error = false;
	}

	void operator()() {
		int socket[2];
		if (socketpair(AF_UNIX, SOCK_STREAM, 0, socket) < 0) {
			throw "socketpair() failed";
		}

		int ret = write(socket[0], _input.c_str(), _input.length());
		if (ret != _input.length()) throw "write() failed";

		_pid = fork();
		if (_pid == -1) {
			close(socket[0]);
			close(socket[1]);
			throw "pair() failed";
		}
		if (_pid == 0) {
			// child
			const char** argv = get_c_args();
			close(socket[0]);
			dup2(socket[1], STDIN_FILENO);
			dup2(socket[1], STDOUT_FILENO);
			dup2(socket[1], STDERR_FILENO);

			execv(argv[0], (char* const *) argv);
			close(socket[1]);
			free(argv);
			exit(-1);
		}
		// parent
		close(socket[1]);
		_socket = socket[0];
	}

	void wait() {
		assert(_socket);
		waitpid(_pid, &_status, 0);
	}

	bool error() {
		return _error;
	}

	string read() {
		const size_t SIZE = 4096;
		assert(_socket);
		stringstream ss;
		char buf[SIZE];
		while (true) {
			int r = ::read(_socket, buf, SIZE);
			if (r == 0) break;
			if (r < 0) {
				_error = true;
				return "";
			}
			ss << string(buf, r);
		}
		return ss.str();
	}

protected:
	void prepare_input(const vector<string>& input) {
		_input = Marshalled(input).str();
	}

	const char** get_c_args() {
		const char** retval = new const char*[_argv.size() + 1];

		for (size_t i = 0; i < _argv.size(); ++i) {
			retval[i] = _argv[i].c_str();
		}
		retval[_argv.size()] = nullptr;
		return retval;
	}

	vector<string> _argv;
	int _status;
	pid_t _pid;
	string _input;
	int _socket;
	bool _error;
};

}  // namespace ib

#endif  // __IB__RUN__H__
