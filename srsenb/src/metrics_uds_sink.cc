/**
 * Copyright 2013-2023 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#include "srsenb/hdr/metrics_uds_sink.h"

#include <cerrno>
#include <cstddef>
#include <cstring>
#include <fcntl.h>
#include <sys/socket.h>
#include <unistd.h>

using namespace srslog;

static bool is_transient_send_error(int err)
{
  switch (err) {
    case EAGAIN:
#if defined(EWOULDBLOCK) && (EWOULDBLOCK != EAGAIN)
    case EWOULDBLOCK:
#endif
    case ENOBUFS:
    case ENOENT:
    case ECONNREFUSED:
      return true;
    default:
      return false;
  }
}

uds_sink::uds_sink(std::string path_, std::unique_ptr<log_formatter> f) : sink(std::move(f)), path(std::move(path_))
{
  std::memset(&addr, 0, sizeof(addr));
  addr.sun_family = AF_UNIX;

  if (path.empty()) {
    init_error = "UDS path is empty";
    return;
  }

  const bool is_abstract = (path[0] == '@');
  const std::size_t max_len = sizeof(addr.sun_path) - 1;

  if (is_abstract) {
    const std::size_t abstract_len = path.size() - 1;
    if (abstract_len > max_len) {
      init_error = "UDS abstract path is too long";
      return;
    }
    addr.sun_path[0] = '\0';
    std::memcpy(addr.sun_path + 1, path.data() + 1, abstract_len);
    addr_len = static_cast<socklen_t>(offsetof(sockaddr_un, sun_path) + 1 + abstract_len);
  } else {
    if (path.size() > max_len) {
      init_error = "UDS path is too long";
      return;
    }
    std::memcpy(addr.sun_path, path.c_str(), path.size() + 1);
    addr_len = static_cast<socklen_t>(offsetof(sockaddr_un, sun_path) + path.size() + 1);
  }

  ready = init_socket();
}

uds_sink::~uds_sink()
{
  if (fd >= 0) {
    ::close(fd);
  }
}

bool uds_sink::init_socket()
{
  fd = ::socket(AF_UNIX, SOCK_DGRAM | SOCK_CLOEXEC, 0);
  if (fd < 0) {
    init_error = std::string("UDS socket() failed: ") + std::strerror(errno);
    return false;
  }

  const int flags = ::fcntl(fd, F_GETFL, 0);
  if (flags >= 0) {
    ::fcntl(fd, F_SETFL, flags | O_NONBLOCK);
  }

  return true;
}

srslog::detail::error_string uds_sink::report_init_error_once()
{
  if (init_error_reported || init_error.empty()) {
    return {};
  }
  init_error_reported = true;
  return detail::error_string(init_error);
}

srslog::detail::error_string uds_sink::write(detail::memory_buffer buffer)
{
  if (!ready) {
    return report_init_error_once();
  }

  const ssize_t ret = ::sendto(fd, buffer.data(), buffer.size(), MSG_NOSIGNAL, (const sockaddr*)&addr, addr_len);
  if (ret < 0) {
    const int err = errno;
    if (is_transient_send_error(err)) {
      return {};
    }
    return detail::error_string(std::string("UDS sendto() failed: ") + std::strerror(err));
  }

  return {};
}
