/**
 * Copyright 2013-2023 Software Radio Systems Limited
 *
 * EPC metrics collector and UDS JSON reporter.
 */

#include "srsepc/hdr/metrics/epc_metrics.h"

#include <algorithm>
#include <chrono>
#include <cerrno>
#include <cmath>
#include <cstddef>
#include <cstring>
#include <fcntl.h>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <sys/socket.h>
#include <sys/sysinfo.h>
#include <sys/un.h>
#include <unistd.h>

namespace srsepc {

namespace {

constexpr uint8_t default_qci = 9;

uint64_t load_counter(const std::atomic<uint64_t>& v)
{
  return v.load(std::memory_order_relaxed);
}

std::string json_escape(const std::string& value)
{
  std::ostringstream out;
  for (const char c : value) {
    switch (c) {
      case '\\': out << "\\\\"; break;
      case '"': out << "\\\""; break;
      case '\b': out << "\\b"; break;
      case '\f': out << "\\f"; break;
      case '\n': out << "\\n"; break;
      case '\r': out << "\\r"; break;
      case '\t': out << "\\t"; break;
      default:
        if (static_cast<unsigned char>(c) < 0x20) {
          out << "\\u" << std::hex << std::setw(4) << std::setfill('0') << static_cast<int>(c);
        } else {
          out << c;
        }
    }
  }
  return out.str();
}

uint64_t now_unix_micros()
{
  const auto now = std::chrono::system_clock::now().time_since_epoch();
  return std::chrono::duration_cast<std::chrono::microseconds>(now).count();
}

bool is_transient_uds_error(int err)
{
  switch (err) {
    case EAGAIN:
#if defined(EWOULDBLOCK) && (EWOULDBLOCK != EAGAIN)
    case EWOULDBLOCK:
#endif
    case ENOENT:
    case ENOBUFS:
    case ECONNREFUSED:
      return true;
    default:
      return false;
  }
}

uint64_t parse_proc_status_kb(const std::string& key)
{
  std::ifstream file("/proc/self/status");
  std::string line;
  while (std::getline(file, line)) {
    if (line.rfind(key, 0) == 0) {
      std::istringstream in(line.substr(key.size()));
      uint64_t value = 0;
      in >> value;
      return value;
    }
  }
  return 0;
}

} // namespace

epc_metrics_collector& epc_metrics_collector::instance()
{
  static epc_metrics_collector inst;
  return inst;
}

void epc_metrics_collector::inc_attach_attempt() { eps_attach_att.fetch_add(1, std::memory_order_relaxed); }
void epc_metrics_collector::inc_attach_success() { eps_attach_succ.fetch_add(1, std::memory_order_relaxed); }
void epc_metrics_collector::inc_detach_attempt() { eps_detach_att.fetch_add(1, std::memory_order_relaxed); }
void epc_metrics_collector::inc_detach_success() { eps_detach_succ.fetch_add(1, std::memory_order_relaxed); }
void epc_metrics_collector::inc_tau_attempt() { tau_intra_sgw_att.fetch_add(1, std::memory_order_relaxed); }
void epc_metrics_collector::inc_tau_success() { tau_intra_sgw_succ.fetch_add(1, std::memory_order_relaxed); }
void epc_metrics_collector::inc_paging_attempt() { paging_eps_att.fetch_add(1, std::memory_order_relaxed); }
void epc_metrics_collector::inc_paging_success() { paging_eps_succ.fetch_add(1, std::memory_order_relaxed); }
void epc_metrics_collector::inc_hss_auth_attempt() { auth_info_hss_att.fetch_add(1, std::memory_order_relaxed); }
void epc_metrics_collector::inc_hss_auth_success() { auth_info_hss_succ.fetch_add(1, std::memory_order_relaxed); }
void epc_metrics_collector::inc_service_request_attempt() { service_req_att.fetch_add(1, std::memory_order_relaxed); }
void epc_metrics_collector::inc_service_request_success() { service_req_succ.fetch_add(1, std::memory_order_relaxed); }
void epc_metrics_collector::inc_create_default_bearer_s11_attempt()
{
  create_bearer_s11_att.fetch_add(1, std::memory_order_relaxed);
}

void epc_metrics_collector::inc_attach_failure(uint8_t cause)
{
  std::lock_guard<std::mutex> lock(mu);
  attach_fail_by_cause[cause]++;
}

void epc_metrics_collector::inc_s1mme_in(uint64_t octets)
{
  s1mme_in_pkts.fetch_add(1, std::memory_order_relaxed);
  s1mme_in_octets.fetch_add(octets, std::memory_order_relaxed);
}

void epc_metrics_collector::inc_s1mme_out()
{
  s1mme_out_pkts.fetch_add(1, std::memory_order_relaxed);
}

void epc_metrics_collector::bearer_activated(uint8_t qci)
{
  std::lock_guard<std::mutex> lock(mu);
  active_bearers_by_qci[qci == 0 ? default_qci : qci]++;
}

void epc_metrics_collector::bearer_released(uint8_t qci)
{
  std::lock_guard<std::mutex> lock(mu);
  const uint8_t normalized_qci = qci == 0 ? default_qci : qci;
  auto it = active_bearers_by_qci.find(normalized_qci);
  if (it == active_bearers_by_qci.end() || it->second == 0) {
    return;
  }
  it->second--;
}

void epc_metrics_collector::inc_s1u_in(uint64_t octets)
{
  s1u_in_pkts.fetch_add(1, std::memory_order_relaxed);
  s1u_in_octets.fetch_add(octets, std::memory_order_relaxed);
}

void epc_metrics_collector::inc_s1u_out(uint64_t octets)
{
  s1u_out_pkts.fetch_add(1, std::memory_order_relaxed);
  s1u_out_octets.fetch_add(octets, std::memory_order_relaxed);
}

void epc_metrics_collector::inc_sgi_in(uint64_t octets)
{
  sgi_in_octets.fetch_add(octets, std::memory_order_relaxed);
}

void epc_metrics_collector::inc_sgi_out(uint64_t octets)
{
  sgi_out_octets.fetch_add(octets, std::memory_order_relaxed);
}

epc_metrics_snapshot_t epc_metrics_collector::snapshot() const
{
  epc_metrics_snapshot_t snap;
  snap.eps_attach_att        = load_counter(eps_attach_att);
  snap.eps_attach_succ       = load_counter(eps_attach_succ);
  snap.eps_detach_att        = load_counter(eps_detach_att);
  snap.eps_detach_succ       = load_counter(eps_detach_succ);
  snap.tau_intra_sgw_att     = load_counter(tau_intra_sgw_att);
  snap.tau_intra_sgw_succ    = load_counter(tau_intra_sgw_succ);
  snap.paging_eps_att        = load_counter(paging_eps_att);
  snap.paging_eps_succ       = load_counter(paging_eps_succ);
  snap.auth_info_hss_att     = load_counter(auth_info_hss_att);
  snap.auth_info_hss_succ    = load_counter(auth_info_hss_succ);
  snap.s1mme_in_pkts         = load_counter(s1mme_in_pkts);
  snap.s1mme_out_pkts        = load_counter(s1mme_out_pkts);
  snap.s1mme_in_octets       = load_counter(s1mme_in_octets);
  snap.service_req_att       = load_counter(service_req_att);
  snap.service_req_succ      = load_counter(service_req_succ);
  snap.create_bearer_s11_att = load_counter(create_bearer_s11_att);
  snap.s1u_in_octets         = load_counter(s1u_in_octets);
  snap.s1u_out_octets        = load_counter(s1u_out_octets);
  snap.s1u_in_pkts           = load_counter(s1u_in_pkts);
  snap.s1u_out_pkts          = load_counter(s1u_out_pkts);
  snap.sgi_in_octets         = load_counter(sgi_in_octets);
  snap.sgi_out_octets        = load_counter(sgi_out_octets);

  std::lock_guard<std::mutex> lock(mu);
  snap.attach_fail_by_cause = attach_fail_by_cause;
  for (const auto& kv : active_bearers_by_qci) {
    snap.active_bearer_mean_by_qci[kv.first] = static_cast<double>(kv.second);
  }
  return snap;
}

epc_metrics_reporter::epc_metrics_reporter() = default;

epc_metrics_reporter::~epc_metrics_reporter()
{
  stop();
}

bool epc_metrics_reporter::start(const epc_metrics_args_t& args_)
{
  if (!args_.report_json_uds_enable) {
    return true;
  }
  if (running.exchange(true)) {
    return true;
  }
  args = args_;
  if (!init_socket()) {
    running = false;
    return false;
  }
  worker = std::thread(&epc_metrics_reporter::run, this);
  return true;
}

void epc_metrics_reporter::stop()
{
  if (!running.exchange(false)) {
    return;
  }
  if (worker.joinable()) {
    worker.join();
  }
  if (fd >= 0) {
    ::close(fd);
    fd = -1;
  }
}

bool epc_metrics_reporter::init_socket()
{
  fd = ::socket(AF_UNIX, SOCK_DGRAM | SOCK_CLOEXEC, 0);
  if (fd < 0) {
    return false;
  }
  const int flags = ::fcntl(fd, F_GETFL, 0);
  if (flags >= 0) {
    ::fcntl(fd, F_SETFL, flags | O_NONBLOCK);
  }
  return true;
}

void epc_metrics_reporter::run()
{
  const double period = std::max(0.1f, args.metrics_period_secs);
  const auto sleep_duration = std::chrono::duration<double>(period);
  while (running.load()) {
    auto eqpt = sample_equipment();
    auto snap = epc_metrics_collector::instance().snapshot();
    send_json(build_json(snap, eqpt));
    std::this_thread::sleep_for(sleep_duration);
  }
}

void epc_metrics_reporter::send_json(const std::string& json)
{
  if (fd < 0 || args.report_json_uds_path.empty()) {
    return;
  }

  sockaddr_un addr{};
  addr.sun_family = AF_UNIX;
  const std::size_t max_path = sizeof(addr.sun_path) - 1;
  if (args.report_json_uds_path.size() > max_path) {
    return;
  }
  std::memcpy(addr.sun_path, args.report_json_uds_path.c_str(), args.report_json_uds_path.size() + 1);
  const socklen_t addr_len = static_cast<socklen_t>(offsetof(sockaddr_un, sun_path) + args.report_json_uds_path.size() + 1);

  const ssize_t ret = ::sendto(fd, json.data(), json.size(), MSG_NOSIGNAL, reinterpret_cast<const sockaddr*>(&addr), addr_len);
  if (ret < 0 && !is_transient_uds_error(errno)) {
    return;
  }
}

std::string epc_metrics_reporter::build_json(const epc_metrics_snapshot_t& snap, const equipment_metrics_t& eqpt) const
{
  const uint64_t ts_us = now_unix_micros();
  std::ostringstream out;
  out << std::fixed << std::setprecision(6);
  out << "{\"type\":\"epc_metrics\",\"epc_id\":\"" << json_escape(args.epc_id) << "\",\"timestamp\":"
      << static_cast<double>(ts_us) / 1000000.0 << ",\"metrics\":{";

  bool first = true;
  auto add = [&](const char* key, uint64_t value) {
    if (!first) {
      out << ',';
    }
    first = false;
    out << '\"' << key << "\":" << value;
  };
  auto add_double = [&](const char* key, double value) {
    if (!first) {
      out << ',';
    }
    first = false;
    out << '\"' << key << "\":" << value;
  };

  add("MM.EpsAttachAtt.E", snap.eps_attach_att);
  add("MM.EpsAttachSucc.E", snap.eps_attach_succ);

  if (!first) {
    out << ',';
  }
  first = false;
  out << "\"MM.EpsAttachFail.Cause.E\":{";
  bool first_cause = true;
  for (const auto& kv : snap.attach_fail_by_cause) {
    if (!first_cause) {
      out << ',';
    }
    first_cause = false;
    out << '\"' << static_cast<unsigned>(kv.first) << "\":" << kv.second;
  }
  out << '}';

  add("MM.EpsDetachUeAttMM.EpsDetachMMEAtt", snap.eps_detach_att);
  add("MM.EpsDetachUeSuccMM.EpsDetachMMESucc", snap.eps_detach_succ);
  add("MM.TauIntraSgwAtt", snap.tau_intra_sgw_att);
  add("MM.TauIntraSgwSucc", snap.tau_intra_sgw_succ);
  add("MM.PagingEpsAtt", snap.paging_eps_att);
  add("MM.PagingEpsSucc", snap.paging_eps_succ);
  add("SEC.AuthInfoHssAtt", snap.auth_info_hss_att);
  add("SEC.AuthInfoHssSucc", snap.auth_info_hss_succ);
  add("IP.IncDataPktS1MME", snap.s1mme_in_pkts);
  add("IP.OutDataPktS1MME", snap.s1mme_out_pkts);
  add("IP.IncDataOctS1MME", snap.s1mme_in_octets);
  add("SM.EpsServiceReqAtt", snap.service_req_att);
  add("SM.EpsServiceReqSucc", snap.service_req_succ);
  add("SM.CreationDefaultBearerS11Att", snap.create_bearer_s11_att);

  out << ",\"SM.ActEPSBearNbrMean.QCI\":{";
  bool first_qci = true;
  for (const auto& kv : snap.active_bearer_mean_by_qci) {
    if (!first_qci) {
      out << ',';
    }
    first_qci = false;
    out << '\"' << static_cast<unsigned>(kv.first) << "\":" << kv.second;
  }
  out << '}';

  add("GTP.InDataOctS1USGW", snap.s1u_in_octets);
  add("GTP.OutDataOctS1USGW", snap.s1u_out_octets);
  add("GTP.InDataPktS1USGW", snap.s1u_in_pkts);
  add("GTP.OutDataPktS1USGW", snap.s1u_out_pkts);
  add("IP.SGiIncLinkUsage", snap.sgi_in_octets);
  add("IP.SGiOutLinkUsage", snap.sgi_out_octets);
  add_double("EQPT.MeanProcessorUsage.ID", eqpt.cpu_percent);
  add("EQPT.MemMeanUsage.ID", eqpt.memory_kb);
  add_double("EQPT.DiskMeanUsage", eqpt.disk_bytes_per_sec);

  out << "}}\n";
  return out.str();
}

epc_metrics_reporter::equipment_metrics_t epc_metrics_reporter::sample_equipment()
{
  equipment_metrics_t eqpt;
  process_sample_t current;

  {
    std::ifstream stat("/proc/self/stat");
    std::string line;
    if (std::getline(stat, line)) {
      const auto end_comm = line.rfind(')');
      if (end_comm != std::string::npos && end_comm + 2 < line.size()) {
        std::istringstream fields(line.substr(end_comm + 2));
        std::string token;
        for (int field = 3; fields >> token; ++field) {
          if (field == 14) {
            current.cpu_ticks += std::stoull(token);
          } else if (field == 15) {
            current.cpu_ticks += std::stoull(token);
            break;
          }
        }
      }
    }
  }

  {
    std::ifstream io("/proc/self/io");
    std::string key;
    uint64_t value = 0;
    while (io >> key >> value) {
      if (key == "read_bytes:") {
        current.read_bytes = value;
      } else if (key == "write_bytes:") {
        current.write_bytes = value;
      }
    }
  }

  eqpt.memory_kb = parse_proc_status_kb("VmRSS:");

  if (have_last_process_sample) {
    const double elapsed = std::max(0.1f, args.metrics_period_secs);
    const double ticks_per_second = static_cast<double>(::sysconf(_SC_CLK_TCK));
    const double cpus = static_cast<double>(std::max<long>(1, ::sysconf(_SC_NPROCESSORS_ONLN)));
    if (current.cpu_ticks >= last_process_sample.cpu_ticks && ticks_per_second > 0) {
      eqpt.cpu_percent = ((current.cpu_ticks - last_process_sample.cpu_ticks) * 100.0) / (ticks_per_second * elapsed * cpus);
    }
    const uint64_t current_disk = current.read_bytes + current.write_bytes;
    const uint64_t last_disk = last_process_sample.read_bytes + last_process_sample.write_bytes;
    if (current_disk >= last_disk) {
      eqpt.disk_bytes_per_sec = static_cast<double>(current_disk - last_disk) / elapsed;
    }
  }

  last_process_sample = current;
  have_last_process_sample = true;
  return eqpt;
}

} // namespace srsepc
