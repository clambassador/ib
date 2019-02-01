#include "ib/logger.h"

namespace ib {

unique_ptr<mutex> Logger::_mutex(new mutex());
int Logger::_cur_fd = 0;
bool Logger::_error_to_file = false;
unordered_map<int, FILE *> Logger::_fd_to_file;

}  // namespace ib
