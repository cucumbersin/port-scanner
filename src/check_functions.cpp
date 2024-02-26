#include <cstring>

#include "check_functions.h"

bool argument_check(int argc, char const *argv[]) {
  if (argc != 4) return false;
  if (ip_check(argv[1]) && port_check(argv[2], argv[3])) return true;
  return false;
}

bool ip_check(const char *ip) {
  size_t size = std::strlen(ip);
  if (size > 16 || size < 7) return false;

  int buf = 0;
  int count_point = 0;
  bool previous_poin = true;
  for (size_t i = 0; i < size; i++) {
    if (ip[i] == '.') {
      ++count_point;
      if (previous_poin) return false;
      previous_poin = true;
      if (buf > 255) return false;
      buf = 0;
    } else if (ip[i] >= '0' && ip[i] <= '9') {
      buf *= 10;
      buf += ip[i] - '0';
      previous_poin = false;
    } else
      return false;
  }
  if (count_point != 3) return false;
  return true;
}

// проверка обоих портов
bool port_check(const char *port_first, const char *port_last) {
  size_t port_first_size = std::strlen(port_first);
  size_t port_last_size = std::strlen(port_last);
  if ((port_first_size > 6 || port_first_size == 0) ||
      (port_last_size > 6 || port_last_size == 0)) {
    return false;
  }

  size_t port_first_buf = 0;
  for (size_t i = 0; i < port_first_size; i++) {
    if (port_first[i] < '0' || port_first[i] > '9') return false;
    port_first_buf *= 10;
    port_first_buf += port_first[i] - '0';
  }
  if (port_first_buf > 65535) return false;

  size_t port_last_buf = 0;
  for (size_t i = 0; i < port_last_size; i++) {
    if (port_last[i] < '0' || port_last[i] > '9') return false;
    port_last_buf *= 10;
    port_last_buf += port_last[i] - '0';
  }
  if (port_last_buf > 65535) return false;

  if (port_first_buf > port_last_buf) return false;
  return true;
}