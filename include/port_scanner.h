#include <arpa/inet.h>
#include <sys/epoll.h>

#include <mutex>
#include <set>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

class Port_scanner {
 public:
  Port_scanner(std::string& ip);
  std::vector<int> scan(int begin_port, int end_port);

 private:
  void task_scan(int begin_port, int end_port);
  std::vector<int> read_epoll(int epollfd, int port_count);
  std::mutex _mutex;
  int number_thread;
  std::vector<std::thread> vec_thread;
  in_addr bin_ip;
  const static int max_events = 1000;
  int ports_on_thread;
  std::vector<int> open_port;
};