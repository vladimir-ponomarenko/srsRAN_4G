#ifndef SRSENB_METRICS_UDS_SINK_H
#define SRSENB_METRICS_UDS_SINK_H

#include "srsran/srslog/sink.h"
#include <string>
#include <sys/socket.h>
#include <sys/un.h>

namespace srslog {

class uds_sink final : public sink
{
public:
  uds_sink(std::string path_, std::unique_ptr<log_formatter> f);
  ~uds_sink() override;

  uds_sink(const uds_sink&) = delete;
  uds_sink& operator=(const uds_sink&) = delete;

  detail::error_string write(detail::memory_buffer buffer) override;
  detail::error_string flush() override { return {}; }

  const std::string& get_path() const { return path; }

private:
  bool               init_socket();
  detail::error_string report_init_error_once();

  std::string path;
  std::string init_error;
  int         fd = -1;
  sockaddr_un addr{};
  socklen_t   addr_len = 0;
  bool        ready = false;
  bool        init_error_reported = false;
};

} // namespace srslog

#endif // SRSENB_METRICS_UDS_SINK_H
