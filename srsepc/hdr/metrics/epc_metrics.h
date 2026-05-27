/**
 * Copyright 2013-2023 Software Radio Systems Limited
 *
 * EPC metrics collector and UDS reporter used by the NMS lab integration.
 */

#ifndef SRSEPC_METRICS_EPC_METRICS_H
#define SRSEPC_METRICS_EPC_METRICS_H

#include <atomic>
#include <cstdint>
#include <map>
#include <mutex>
#include <string>
#include <thread>

namespace srsepc {

struct epc_metrics_args_t {
  float       metrics_period_secs     = 1.0f;
  bool        report_json_uds_enable  = false;
  std::string report_json_uds_path    = "/tmp/epc_metrics.uds";
  std::string epc_id                  = "EPC";
};

struct epc_metrics_snapshot_t {
  uint64_t eps_attach_att         = 0;
  uint64_t eps_attach_succ        = 0;
  uint64_t eps_detach_att         = 0;
  uint64_t eps_detach_succ        = 0;
  uint64_t tau_intra_sgw_att      = 0;
  uint64_t tau_intra_sgw_succ     = 0;
  uint64_t paging_eps_att         = 0;
  uint64_t paging_eps_succ        = 0;
  uint64_t auth_info_hss_att      = 0;
  uint64_t auth_info_hss_succ     = 0;
  uint64_t s1mme_in_pkts          = 0;
  uint64_t s1mme_out_pkts         = 0;
  uint64_t s1mme_in_octets        = 0;
  uint64_t service_req_att        = 0;
  uint64_t service_req_succ       = 0;
  uint64_t create_bearer_s11_att  = 0;
  uint64_t s1u_in_octets          = 0;
  uint64_t s1u_out_octets         = 0;
  uint64_t s1u_in_pkts            = 0;
  uint64_t s1u_out_pkts           = 0;
  uint64_t sgi_in_octets          = 0;
  uint64_t sgi_out_octets         = 0;

  std::map<uint8_t, uint64_t> attach_fail_by_cause;
  std::map<uint8_t, double>   active_bearer_mean_by_qci;
};

class epc_metrics_collector
{
public:
  static epc_metrics_collector& instance();

  void inc_attach_attempt();
  void inc_attach_success();
  void inc_attach_failure(uint8_t cause);
  void inc_detach_attempt();
  void inc_detach_success();
  void inc_tau_attempt();
  void inc_tau_success();
  void inc_paging_attempt();
  void inc_paging_success();
  void inc_hss_auth_attempt();
  void inc_hss_auth_success();
  void inc_s1mme_in(uint64_t octets);
  void inc_s1mme_out();
  void inc_service_request_attempt();
  void inc_service_request_success();
  void inc_create_default_bearer_s11_attempt();
  void bearer_activated(uint8_t qci);
  void bearer_released(uint8_t qci);
  void inc_s1u_in(uint64_t octets);
  void inc_s1u_out(uint64_t octets);
  void inc_sgi_in(uint64_t octets);
  void inc_sgi_out(uint64_t octets);

  epc_metrics_snapshot_t snapshot() const;

private:
  epc_metrics_collector() = default;

  std::atomic<uint64_t> eps_attach_att{0};
  std::atomic<uint64_t> eps_attach_succ{0};
  std::atomic<uint64_t> eps_detach_att{0};
  std::atomic<uint64_t> eps_detach_succ{0};
  std::atomic<uint64_t> tau_intra_sgw_att{0};
  std::atomic<uint64_t> tau_intra_sgw_succ{0};
  std::atomic<uint64_t> paging_eps_att{0};
  std::atomic<uint64_t> paging_eps_succ{0};
  std::atomic<uint64_t> auth_info_hss_att{0};
  std::atomic<uint64_t> auth_info_hss_succ{0};
  std::atomic<uint64_t> s1mme_in_pkts{0};
  std::atomic<uint64_t> s1mme_out_pkts{0};
  std::atomic<uint64_t> s1mme_in_octets{0};
  std::atomic<uint64_t> service_req_att{0};
  std::atomic<uint64_t> service_req_succ{0};
  std::atomic<uint64_t> create_bearer_s11_att{0};
  std::atomic<uint64_t> s1u_in_octets{0};
  std::atomic<uint64_t> s1u_out_octets{0};
  std::atomic<uint64_t> s1u_in_pkts{0};
  std::atomic<uint64_t> s1u_out_pkts{0};
  std::atomic<uint64_t> sgi_in_octets{0};
  std::atomic<uint64_t> sgi_out_octets{0};

  mutable std::mutex         mu;
  std::map<uint8_t, uint64_t> attach_fail_by_cause;
  std::map<uint8_t, uint64_t> active_bearers_by_qci;
};

class epc_metrics_reporter
{
public:
  epc_metrics_reporter();
  ~epc_metrics_reporter();

  epc_metrics_reporter(const epc_metrics_reporter&) = delete;
  epc_metrics_reporter& operator=(const epc_metrics_reporter&) = delete;

  bool start(const epc_metrics_args_t& args);
  void stop();

private:
  struct process_sample_t {
    uint64_t cpu_ticks  = 0;
    uint64_t read_bytes = 0;
    uint64_t write_bytes = 0;
  };

  struct equipment_metrics_t {
    double   cpu_percent       = 0.0;
    uint64_t memory_kb         = 0;
    double   disk_bytes_per_sec = 0.0;
  };

  void run();
  bool init_socket();
  void send_json(const std::string& json);
  std::string build_json(const epc_metrics_snapshot_t& snap, const equipment_metrics_t& eqpt) const;
  equipment_metrics_t sample_equipment();

  epc_metrics_args_t args;
  std::atomic<bool>  running{false};
  std::thread        worker;
  int                fd = -1;
  process_sample_t   last_process_sample{};
  bool               have_last_process_sample = false;
};

} // namespace srsepc

#endif // SRSEPC_METRICS_EPC_METRICS_H
