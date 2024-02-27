#include <errno.h>
#include <fcntl.h>
#include <netinet/tcp.h>
#include <sys/epoll.h>
#include <unistd.h>

#include <fstream>
#include <iostream>
#include <set>
#include <stdexcept>

#include "port_scanner.h"

Port_scanner::Port_scanner(std::string& ip)
    : number_thread(sysconf(_SC_NPROCESSORS_ONLN)) {
  if (inet_pton(AF_INET, ip.c_str(), &bin_ip) < 0) {
    throw std::invalid_argument("inet_pton:" + std::to_string(errno) + '\n');
  }
}

std::vector<int> Port_scanner::scan(int first_port, int last_port) {
  //проверка на маленькое количество протов надо добавить;
  vec_thread.resize(number_thread);
  int first_port_for_task = first_port;
  if (last_port - first_port + 1 > number_thread) {
    int step = (last_port - first_port + 1) / vec_thread.size();
    ports_on_thread = step;
    for (size_t i = 0; i < vec_thread.size() - 1; i++) {
      vec_thread[i] =
          std::thread(&Port_scanner::task_scan, this, first_port_for_task,
                      first_port_for_task + step);
      first_port_for_task = first_port_for_task + step;
    }
    vec_thread.back() = std::thread(&Port_scanner::task_scan, this,
                                    first_port_for_task, last_port);

    for (auto& elem : vec_thread) {
      elem.join();
    }
  } else {
    task_scan(first_port, last_port);
  }
  return open_port;
}

void Port_scanner::task_scan(int begin_port_loop, int end_port_loop) {  
    int epollfd = epoll_create1(0);
    std::unordered_map<int, int> sock_port_map;
    if (epollfd < 0)
      throw std::runtime_error("epoll_create1:" + std::to_string(errno));
    std::set<int> port;
    int port_count = end_port_loop - begin_port_loop + 1;
    try {
      for (size_t i = begin_port_loop; i <= end_port_loop; i++) {
        int socketfd = socket(AF_INET, SOCK_STREAM, 0);
        if (socketfd <= 0) {
          throw std::runtime_error("socket :" + std::to_string(errno));
        }

        sockaddr_in server_address;
        server_address.sin_family = AF_INET;
        server_address.sin_addr = bin_ip;
        server_address.sin_port = htons(i);
        epoll_event ev;
        ev.events = EPOLLOUT | EPOLLIN ;//| EPOLLERR;
        ev.data.fd = socketfd;

        //установка ограничения на количество повторных syn запросов
        int opt = 1;
        int opt_len = sizeof(opt);
        setsockopt(socketfd, IPPROTO_TCP, TCP_SYNCNT, &opt, opt_len);


        sock_port_map.emplace(socketfd, i);

        //установка в не блокирующее состояние
        fcntl(socketfd, F_SETFL, O_NONBLOCK);

        int connect_status =
            connect(socketfd, (struct sockaddr*)&server_address,
                    sizeof(server_address));
        //добовление в epoll
        if (epoll_ctl(epollfd, EPOLL_CTL_ADD, socketfd, &ev) < 0) {
          throw std::runtime_error("  " + std::to_string(i) +
                                   "epoll_ctl:" + std::to_string(errno));
        }
      }
      std::vector<int> open_sock = read_epoll(epollfd, port_count);
      for (auto elem : open_sock) {
        std::lock_guard<std::mutex> lock(_mutex);
        open_port.push_back(sock_port_map[elem]);
      }
      for(auto elem: sock_port_map){
        close(elem.first);
      }

    } catch (const std::exception& e) {
      std::cerr << e.what() << '\n';
    }  
}

std::vector<int> Port_scanner::read_epoll(int epollfd, int port_count) {
  std::vector<int> open_sock;
  epoll_event* events = new epoll_event[port_count];
  int count_events = 0;
  do {
    //std::cout << "start\n";
    count_events = epoll_wait(epollfd, events, port_count, 2000);
    if (count_events == -1) {
      throw std::runtime_error("epoll_wait:" + std::to_string(errno));
    } else if(count_events == 0) break;
    else {
      for (int i = 0; i < count_events; ++i) {
        int sockfd = events[i].data.fd;
        int socketError;
        socklen_t len = sizeof socketError;
        getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &socketError,
                   &len);  //состояние сокета
        if (socketError == 0) {
          open_sock.push_back(sockfd);
        }
        close(sockfd);
        //--port_count;
      }
    }
  } while (true);//port_count > 0);
  delete[] events;
  return open_sock;
}
