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

#ifndef SRSENB_RRC_METRICS_H
#define SRSENB_RRC_METRICS_H

#include "srsenb/hdr/common/common_enb.h"
#include <cstdint>
#include <string>
#include <vector>

namespace srsenb {

typedef enum {
  RRC_STATE_IDLE = 0,
  RRC_STATE_WAIT_FOR_CON_SETUP_COMPLETE,
  RRC_STATE_WAIT_FOR_CON_REEST_COMPLETE,
  RRC_STATE_WAIT_FOR_SECURITY_MODE_COMPLETE,
  RRC_STATE_WAIT_FOR_UE_CAP_INFO,
  RRC_STATE_WAIT_FOR_UE_CAP_INFO_ENDC, /* only entered for UEs with NSA support */
  RRC_STATE_WAIT_FOR_CON_RECONF_COMPLETE,
  RRC_STATE_REESTABLISHMENT_COMPLETE,
  RRC_STATE_REGISTERED,
  RRC_STATE_RELEASE_REQUEST,
  RRC_STATE_N_ITEMS,
} rrc_state_t;

struct rrc_ue_metrics_t {
  rrc_state_t                                 state;
  uint32_t                                    state_code = 0;
  std::vector<std::pair<uint32_t, uint32_t> > drb_qci_map;
  uint32_t                                    nof_cells = 0;
  bool                                        is_allocated = false;
  bool                                        sr_res_present = false;
  bool                                        n_pucch_cs_present = false;
  bool                                        is_csfb = false;
  bool                                        connect_notified = false;
  uint32_t                                    rlf_cnt = 0;
  bool                                        rlf_info_pending = false;
  uint32_t                                    consecutive_kos_dl = 0;
  uint32_t                                    consecutive_kos_ul = 0;
  bool                                        has_tmsi = false;
  uint32_t                                    m_tmsi = 0;
  uint8_t                                     mmec = 0;
  uint32_t                                    establishment_cause = 0;
  uint32_t                                    transaction_id = 0;
  uint32_t                                    activity_timer_running = 0;
  uint32_t                                    activity_timer_elapsed = 0;
  uint32_t                                    activity_timer_duration = 0;
  uint32_t                                    phy_dl_rlf_timer_running = 0;
  uint32_t                                    phy_dl_rlf_timer_elapsed = 0;
  uint32_t                                    phy_dl_rlf_timer_duration = 0;
  uint32_t                                    phy_ul_rlf_timer_running = 0;
  uint32_t                                    phy_ul_rlf_timer_elapsed = 0;
  uint32_t                                    phy_ul_rlf_timer_duration = 0;
  uint32_t                                    rlc_rlf_timer_running = 0;
  uint32_t                                    rlc_rlf_timer_elapsed = 0;
  uint32_t                                    rlc_rlf_timer_duration = 0;
  uint32_t                                    last_ul_msg_bytes = 0;
  bool                                        eutra_capabilities_unpacked = false;
  std::string                                 rrc_release_cause;
  uint32_t                                    rrc_con_req_rx = 0;
  uint32_t                                    rrc_con_setup_tx = 0;
  uint32_t                                    rrc_con_setup_complete_rx = 0;
  uint32_t                                    rrc_con_reject_tx = 0;
  uint32_t                                    rrc_con_reest_req_rx = 0;
  uint32_t                                    rrc_con_reest_tx = 0;
  uint32_t                                    rrc_con_reest_complete_rx = 0;
  uint32_t                                    rrc_con_reest_reject_tx = 0;
  uint32_t                                    rrc_con_reconf_tx = 0;
  uint32_t                                    rrc_con_reconf_complete_rx = 0;
  uint32_t                                    rrc_con_release_tx = 0;
  uint32_t                                    rrc_security_mode_command_tx = 0;
  uint32_t                                    rrc_security_mode_complete_rx = 0;
  uint32_t                                    rrc_security_mode_failure_rx = 0;
  uint32_t                                    rrc_ue_cap_enquiry_tx = 0;
  uint32_t                                    rrc_ue_cap_info_rx = 0;
  uint32_t                                    rrc_ue_info_req_tx = 0;
  uint32_t                                    rrc_ue_info_resp_rx = 0;
  uint32_t                                    rrc_max_rlc_retx = 0;
  uint32_t                                    rrc_protocol_fail = 0;
};

struct rrc_metrics_t {
  std::vector<rrc_ue_metrics_t> ues;
  uint32_t                      total_ues = 0;
  uint32_t                      connected_ues = 0;
  uint32_t                      rrc_con_req_rx = 0;
  uint32_t                      rrc_con_setup_tx = 0;
  uint32_t                      rrc_con_setup_complete_rx = 0;
  uint32_t                      rrc_con_reject_tx = 0;
  uint32_t                      rrc_con_reest_req_rx = 0;
  uint32_t                      rrc_con_reest_tx = 0;
  uint32_t                      rrc_con_reest_complete_rx = 0;
  uint32_t                      rrc_con_reest_reject_tx = 0;
  uint32_t                      rrc_con_reconf_tx = 0;
  uint32_t                      rrc_con_reconf_complete_rx = 0;
  uint32_t                      rrc_con_release_tx = 0;
  uint32_t                      rrc_security_mode_command_tx = 0;
  uint32_t                      rrc_security_mode_complete_rx = 0;
  uint32_t                      rrc_security_mode_failure_rx = 0;
  uint32_t                      rrc_ue_cap_enquiry_tx = 0;
  uint32_t                      rrc_ue_cap_info_rx = 0;
  uint32_t                      rrc_ue_info_req_tx = 0;
  uint32_t                      rrc_ue_info_resp_rx = 0;
  uint32_t                      rrc_max_rlc_retx = 0;
  uint32_t                      rrc_protocol_fail = 0;
  uint32_t                      paging_requests_total = 0;
  uint32_t                      paging_imsi = 0;
  uint32_t                      paging_tmsi = 0;
  uint32_t                      paging_add_fail = 0;
  uint32_t                      paging_pdu_tx = 0;
  uint32_t                      paging_bytes_tx = 0;
  uint32_t                      paging_identities_tx = 0;
  uint32_t                      pdcp_integrity_errors = 0;
};

} // namespace srsenb

#endif // SRSENB_RRC_METRICS_H
