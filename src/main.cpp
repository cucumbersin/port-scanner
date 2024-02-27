#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/epoll.h>
#include <sys/resource.h>
#include <sys/socket.h>
#include <unistd.h>

#include <algorithm>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

#include "check_functions.h"
#include "port_scanner.h"

int main(int argc, char const *argv[]) {
  //Увеличение количества файловых дескрипторов на процесс
  struct rlimit rl;
  rl.rlim_cur = 65536;
  rl.rlim_max = 65536;
  if (setrlimit(RLIMIT_NOFILE, &rl) == -1) {
    std::cout << "Permission denied\n";
    return 1;
  }

  if (!argument_check(argc, argv)) {
    std::cout << "not valid argument\n"
              << "input format: scanner <IP> <port_first> <port_last>\n";
    return 0;
  }

  std::string ip = argv[1];
  int port_first = atoi(argv[2]), port_last = atoi(argv[3]);

  Port_scanner scanner(ip);
  std::vector<int> open_port = scanner.scan(port_first, port_last);
  std::sort(open_port.begin(), open_port.end());

  if (!open_port.empty()) {
    std::cout << "open ports:\n";
    for (auto elem : open_port) {
      std::cout << elem << '\n';
    }
  } else {
    std::cout << "no open ports\n";
  }
  return 0;
}
