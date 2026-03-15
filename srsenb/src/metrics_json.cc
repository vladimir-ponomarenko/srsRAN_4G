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

#include "srsenb/hdr/metrics_json.h"
#include "srsran/srslog/context.h"

using namespace srsenb;

namespace {

/// Bearer container metrics.
DECLARE_METRIC("bearer_id", metric_bearer_id, uint32_t, "");
DECLARE_METRIC("qci", metric_qci, uint32_t, "");
DECLARE_METRIC("dl_total_bytes", metric_dl_total_bytes, uint64_t, "");
DECLARE_METRIC("ul_total_bytes", metric_ul_total_bytes, uint64_t, "");
DECLARE_METRIC("dl_latency", metric_dl_latency, float, "");
DECLARE_METRIC("ul_latency", metric_ul_latency, float, "");
DECLARE_METRIC("dl_buffered_bytes", metric_dl_buffered_bytes, uint32_t, "");
DECLARE_METRIC("ul_buffered_bytes", metric_ul_buffered_bytes, uint32_t, "");
DECLARE_METRIC_SET("bearer_container",
                   mset_bearer_container,
                   metric_bearer_id,
                   metric_qci,
                   metric_dl_total_bytes,
                   metric_ul_total_bytes,
                   metric_dl_latency,
                   metric_ul_latency,
                   metric_dl_buffered_bytes,
                   metric_ul_buffered_bytes);

/// UE container metrics.
DECLARE_METRIC("ue_rnti", metric_ue_rnti, uint32_t, "");
DECLARE_METRIC("dl_cqi", metric_dl_cqi, float, "");
DECLARE_METRIC("dl_mcs", metric_dl_mcs, float, "");
DECLARE_METRIC("dl_bitrate", metric_dl_bitrate, float, "");
DECLARE_METRIC("dl_bler", metric_dl_bler, float, "");
DECLARE_METRIC("ul_snr", metric_ul_snr, float, "");
DECLARE_METRIC("ul_mcs", metric_ul_mcs, float, "");
DECLARE_METRIC("ul_pusch_rssi", metric_ul_pusch_rssi, float, "");
DECLARE_METRIC("ul_pucch_rssi", metric_ul_pucch_rssi, float, "");
DECLARE_METRIC("ul_pucch_ni", metric_ul_pucch_ni, float, "");
DECLARE_METRIC("ul_pusch_tpc", metric_ul_pusch_tpc, int64_t, "");
DECLARE_METRIC("ul_pucch_tpc", metric_ul_pucch_tpc, int64_t, "");
DECLARE_METRIC("dl_cqi_offset", metric_dl_cqi_offset, float, "");
DECLARE_METRIC("ul_snr_offset", metric_ul_snr_offset, float, "");
DECLARE_METRIC("ul_bitrate", metric_ul_bitrate, float, "");
DECLARE_METRIC("ul_bler", metric_ul_bler, float, "");
DECLARE_METRIC("ul_phr", metric_ul_phr, float, "");
DECLARE_METRIC("ul_bsr", metric_bsr, uint32_t, "");
DECLARE_METRIC("rrc_state_str", metric_rrc_state_str, std::string, "");
DECLARE_METRIC("rrc_state", metric_rrc_state, uint32_t, "");
DECLARE_METRIC("rrc_drb_count", metric_rrc_drb_count, uint32_t, "");
DECLARE_METRIC("rrc_nof_cells", metric_rrc_nof_cells, uint32_t, "");
DECLARE_METRIC("rrc_is_allocated", metric_rrc_is_allocated, uint32_t, "");
DECLARE_METRIC("rrc_sr_res_present", metric_rrc_sr_res_present, uint32_t, "");
DECLARE_METRIC("rrc_n_pucch_cs_present", metric_rrc_n_pucch_cs_present, uint32_t, "");
DECLARE_METRIC("rrc_is_csfb", metric_rrc_is_csfb, uint32_t, "");
DECLARE_METRIC("rrc_connect_notified", metric_rrc_connect_notified, uint32_t, "");
DECLARE_METRIC("rrc_rlf_cnt", metric_rrc_rlf_cnt, uint32_t, "");
DECLARE_METRIC("rrc_rlf_info_pending", metric_rrc_rlf_info_pending, uint32_t, "");
DECLARE_METRIC("rrc_consecutive_kos_dl", metric_rrc_consecutive_kos_dl, uint32_t, "");
DECLARE_METRIC("rrc_consecutive_kos_ul", metric_rrc_consecutive_kos_ul, uint32_t, "");
DECLARE_METRIC("rrc_has_tmsi", metric_rrc_has_tmsi, uint32_t, "");
DECLARE_METRIC("rrc_m_tmsi", metric_rrc_m_tmsi, uint32_t, "");
DECLARE_METRIC("rrc_mmec", metric_rrc_mmec, uint32_t, "");
DECLARE_METRIC("rrc_establishment_cause", metric_rrc_establishment_cause, uint32_t, "");
DECLARE_METRIC("rrc_transaction_id", metric_rrc_transaction_id, uint32_t, "");
DECLARE_METRIC("rrc_activity_timer_running", metric_rrc_activity_timer_running, uint32_t, "");
DECLARE_METRIC("rrc_activity_timer_elapsed", metric_rrc_activity_timer_elapsed, uint32_t, "");
DECLARE_METRIC("rrc_activity_timer_duration", metric_rrc_activity_timer_duration, uint32_t, "");
DECLARE_METRIC("rrc_phy_dl_rlf_timer_running", metric_rrc_phy_dl_rlf_timer_running, uint32_t, "");
DECLARE_METRIC("rrc_phy_dl_rlf_timer_elapsed", metric_rrc_phy_dl_rlf_timer_elapsed, uint32_t, "");
DECLARE_METRIC("rrc_phy_dl_rlf_timer_duration", metric_rrc_phy_dl_rlf_timer_duration, uint32_t, "");
DECLARE_METRIC("rrc_phy_ul_rlf_timer_running", metric_rrc_phy_ul_rlf_timer_running, uint32_t, "");
DECLARE_METRIC("rrc_phy_ul_rlf_timer_elapsed", metric_rrc_phy_ul_rlf_timer_elapsed, uint32_t, "");
DECLARE_METRIC("rrc_phy_ul_rlf_timer_duration", metric_rrc_phy_ul_rlf_timer_duration, uint32_t, "");
DECLARE_METRIC("rrc_rlc_rlf_timer_running", metric_rrc_rlc_rlf_timer_running, uint32_t, "");
DECLARE_METRIC("rrc_rlc_rlf_timer_elapsed", metric_rrc_rlc_rlf_timer_elapsed, uint32_t, "");
DECLARE_METRIC("rrc_rlc_rlf_timer_duration", metric_rrc_rlc_rlf_timer_duration, uint32_t, "");
DECLARE_METRIC("rrc_last_ul_msg_bytes", metric_rrc_last_ul_msg_bytes, uint32_t, "");
DECLARE_METRIC("rrc_eutra_capabilities_unpacked", metric_rrc_eutra_capabilities_unpacked, uint32_t, "");
DECLARE_METRIC("rrc_release_cause", metric_rrc_release_cause, std::string, "");
DECLARE_METRIC("rrc_con_req_rx", metric_rrc_con_req_rx, uint32_t, "");
DECLARE_METRIC("rrc_con_setup_tx", metric_rrc_con_setup_tx, uint32_t, "");
DECLARE_METRIC("rrc_con_setup_complete_rx", metric_rrc_con_setup_complete_rx, uint32_t, "");
DECLARE_METRIC("rrc_con_reject_tx", metric_rrc_con_reject_tx, uint32_t, "");
DECLARE_METRIC("rrc_con_reest_req_rx", metric_rrc_con_reest_req_rx, uint32_t, "");
DECLARE_METRIC("rrc_con_reest_tx", metric_rrc_con_reest_tx, uint32_t, "");
DECLARE_METRIC("rrc_con_reest_complete_rx", metric_rrc_con_reest_complete_rx, uint32_t, "");
DECLARE_METRIC("rrc_con_reest_reject_tx", metric_rrc_con_reest_reject_tx, uint32_t, "");
DECLARE_METRIC("rrc_con_reconf_tx", metric_rrc_con_reconf_tx, uint32_t, "");
DECLARE_METRIC("rrc_con_reconf_complete_rx", metric_rrc_con_reconf_complete_rx, uint32_t, "");
DECLARE_METRIC("rrc_con_release_tx", metric_rrc_con_release_tx, uint32_t, "");
DECLARE_METRIC("rrc_security_mode_command_tx", metric_rrc_security_mode_command_tx, uint32_t, "");
DECLARE_METRIC("rrc_security_mode_complete_rx", metric_rrc_security_mode_complete_rx, uint32_t, "");
DECLARE_METRIC("rrc_security_mode_failure_rx", metric_rrc_security_mode_failure_rx, uint32_t, "");
DECLARE_METRIC("rrc_ue_cap_enquiry_tx", metric_rrc_ue_cap_enquiry_tx, uint32_t, "");
DECLARE_METRIC("rrc_ue_cap_info_rx", metric_rrc_ue_cap_info_rx, uint32_t, "");
DECLARE_METRIC("rrc_ue_info_req_tx", metric_rrc_ue_info_req_tx, uint32_t, "");
DECLARE_METRIC("rrc_ue_info_resp_rx", metric_rrc_ue_info_resp_rx, uint32_t, "");
DECLARE_METRIC("rrc_max_rlc_retx", metric_rrc_max_rlc_retx, uint32_t, "");
DECLARE_METRIC("rrc_protocol_fail", metric_rrc_protocol_fail, uint32_t, "");
DECLARE_METRIC_LIST("bearer_list", mlist_bearers, std::vector<mset_bearer_container>);
DECLARE_METRIC_SET("ue_container",
                   mset_ue_container,
                   metric_ue_rnti,
                   metric_dl_cqi,
                   metric_dl_mcs,
                   metric_ul_pusch_rssi,
                   metric_ul_pucch_rssi,
                   metric_ul_pucch_ni,
                   metric_ul_pusch_tpc,
                   metric_ul_pucch_tpc,
                   metric_dl_cqi_offset,
                   metric_ul_snr_offset,
                   metric_dl_bitrate,
                   metric_dl_bler,
                   metric_ul_snr,
                   metric_ul_mcs,
                   metric_ul_bitrate,
                   metric_ul_bler,
                   metric_ul_phr,
                   metric_bsr,
                   metric_rrc_state_str,
                   metric_rrc_state,
                   metric_rrc_drb_count,
                   metric_rrc_nof_cells,
                   metric_rrc_is_allocated,
                   metric_rrc_sr_res_present,
                   metric_rrc_n_pucch_cs_present,
                   metric_rrc_is_csfb,
                   metric_rrc_connect_notified,
                   metric_rrc_rlf_cnt,
                   metric_rrc_rlf_info_pending,
                   metric_rrc_consecutive_kos_dl,
                   metric_rrc_consecutive_kos_ul,
                   metric_rrc_has_tmsi,
                   metric_rrc_m_tmsi,
                   metric_rrc_mmec,
                   metric_rrc_establishment_cause,
                   metric_rrc_transaction_id,
                   metric_rrc_activity_timer_running,
                   metric_rrc_activity_timer_elapsed,
                   metric_rrc_activity_timer_duration,
                   metric_rrc_phy_dl_rlf_timer_running,
                   metric_rrc_phy_dl_rlf_timer_elapsed,
                   metric_rrc_phy_dl_rlf_timer_duration,
                   metric_rrc_phy_ul_rlf_timer_running,
                   metric_rrc_phy_ul_rlf_timer_elapsed,
                   metric_rrc_phy_ul_rlf_timer_duration,
                   metric_rrc_rlc_rlf_timer_running,
                   metric_rrc_rlc_rlf_timer_elapsed,
                   metric_rrc_rlc_rlf_timer_duration,
                   metric_rrc_last_ul_msg_bytes,
                   metric_rrc_eutra_capabilities_unpacked,
                   metric_rrc_release_cause,
                   metric_rrc_con_req_rx,
                   metric_rrc_con_setup_tx,
                   metric_rrc_con_setup_complete_rx,
                   metric_rrc_con_reject_tx,
                   metric_rrc_con_reest_req_rx,
                   metric_rrc_con_reest_tx,
                   metric_rrc_con_reest_complete_rx,
                   metric_rrc_con_reest_reject_tx,
                   metric_rrc_con_reconf_tx,
                   metric_rrc_con_reconf_complete_rx,
                   metric_rrc_con_release_tx,
                   metric_rrc_security_mode_command_tx,
                   metric_rrc_security_mode_complete_rx,
                   metric_rrc_security_mode_failure_rx,
                   metric_rrc_ue_cap_enquiry_tx,
                   metric_rrc_ue_cap_info_rx,
                   metric_rrc_ue_info_req_tx,
                   metric_rrc_ue_info_resp_rx,
                   metric_rrc_max_rlc_retx,
                   metric_rrc_protocol_fail,
                   mlist_bearers);

/// Cell container metrics.
DECLARE_METRIC("carrier_id", metric_carrier_id, uint32_t, "");
DECLARE_METRIC("pci", metric_pci, uint32_t, "");
DECLARE_METRIC("nof_rach", metric_nof_rach, uint32_t, "");
DECLARE_METRIC_LIST("ue_list", mlist_ues, std::vector<mset_ue_container>);
DECLARE_METRIC_SET("cell_container", mset_cell_container, metric_carrier_id, metric_pci, metric_nof_rach, mlist_ues);

/// Metrics root object.
DECLARE_METRIC("type", metric_type_tag, std::string, "");
DECLARE_METRIC("enb_serial", metric_enb_serial, std::string, "");
DECLARE_METRIC("timestamp", metric_timestamp_tag, double, "");
DECLARE_METRIC_LIST("cell_list", mlist_cell, std::vector<mset_cell_container>);
DECLARE_METRIC("s1ap_status", metric_s1ap_status, std::string, "");
DECLARE_METRIC("s1ap_status_code", metric_s1ap_status_code, uint32_t, "");
DECLARE_METRIC("nas_ul_msgs", metric_nas_ul_msgs, uint64_t, "");
DECLARE_METRIC("nas_ul_fail", metric_nas_ul_fail, uint64_t, "");
DECLARE_METRIC("nas_dl_msgs", metric_nas_dl_msgs, uint64_t, "");
DECLARE_METRIC("nas_dl_drop", metric_nas_dl_drop, uint64_t, "");
DECLARE_METRIC("nas_ul_bytes", metric_nas_ul_bytes, uint64_t, "");
DECLARE_METRIC("nas_dl_bytes", metric_nas_dl_bytes, uint64_t, "");
DECLARE_METRIC("nas_ul_transport_initial_ue", metric_nas_ul_transport_initial_ue, uint64_t, "");
DECLARE_METRIC("nas_ul_transport_ul_nas", metric_nas_ul_transport_ul_nas, uint64_t, "");
DECLARE_METRIC("nas_dl_transport_dl_nas", metric_nas_dl_transport_dl_nas, uint64_t, "");
DECLARE_METRIC("nas_ul_sec_hdr_plain", metric_nas_ul_sec_hdr_plain, uint64_t, "");
DECLARE_METRIC("nas_ul_sec_hdr_integrity", metric_nas_ul_sec_hdr_integrity, uint64_t, "");
DECLARE_METRIC("nas_ul_sec_hdr_integrity_ciphered", metric_nas_ul_sec_hdr_integrity_ciphered, uint64_t, "");
DECLARE_METRIC("nas_ul_sec_hdr_integrity_new_ctx", metric_nas_ul_sec_hdr_integrity_new_ctx, uint64_t, "");
DECLARE_METRIC("nas_ul_sec_hdr_integrity_ciphered_new_ctx",
               metric_nas_ul_sec_hdr_integrity_ciphered_new_ctx,
               uint64_t,
               "");
DECLARE_METRIC("nas_ul_sec_hdr_service_request", metric_nas_ul_sec_hdr_service_request, uint64_t, "");
DECLARE_METRIC("nas_ul_sec_hdr_unknown", metric_nas_ul_sec_hdr_unknown, uint64_t, "");
DECLARE_METRIC("nas_dl_sec_hdr_plain", metric_nas_dl_sec_hdr_plain, uint64_t, "");
DECLARE_METRIC("nas_dl_sec_hdr_integrity", metric_nas_dl_sec_hdr_integrity, uint64_t, "");
DECLARE_METRIC("nas_dl_sec_hdr_integrity_ciphered", metric_nas_dl_sec_hdr_integrity_ciphered, uint64_t, "");
DECLARE_METRIC("nas_dl_sec_hdr_integrity_new_ctx", metric_nas_dl_sec_hdr_integrity_new_ctx, uint64_t, "");
DECLARE_METRIC("nas_dl_sec_hdr_integrity_ciphered_new_ctx",
               metric_nas_dl_sec_hdr_integrity_ciphered_new_ctx,
               uint64_t,
               "");
DECLARE_METRIC("nas_dl_sec_hdr_service_request", metric_nas_dl_sec_hdr_service_request, uint64_t, "");
DECLARE_METRIC("nas_dl_sec_hdr_unknown", metric_nas_dl_sec_hdr_unknown, uint64_t, "");
DECLARE_METRIC("nas_ul_pd_emm", metric_nas_ul_pd_emm, uint64_t, "");
DECLARE_METRIC("nas_ul_pd_esm", metric_nas_ul_pd_esm, uint64_t, "");
DECLARE_METRIC("nas_ul_pd_other", metric_nas_ul_pd_other, uint64_t, "");
DECLARE_METRIC("nas_dl_pd_emm", metric_nas_dl_pd_emm, uint64_t, "");
DECLARE_METRIC("nas_dl_pd_esm", metric_nas_dl_pd_esm, uint64_t, "");
DECLARE_METRIC("nas_dl_pd_other", metric_nas_dl_pd_other, uint64_t, "");
DECLARE_METRIC("nas_ul_pd_last", metric_nas_ul_pd_last, uint32_t, "");
DECLARE_METRIC("nas_dl_pd_last", metric_nas_dl_pd_last, uint32_t, "");
DECLARE_METRIC("nas_ul_short_pdu", metric_nas_ul_short_pdu, uint64_t, "");
DECLARE_METRIC("nas_dl_short_pdu", metric_nas_dl_short_pdu, uint64_t, "");
DECLARE_METRIC("nas_ul_parse_fail", metric_nas_ul_parse_fail, uint64_t, "");
DECLARE_METRIC("nas_dl_parse_fail", metric_nas_dl_parse_fail, uint64_t, "");
DECLARE_METRIC("nas_ul_attach", metric_nas_ul_attach, uint64_t, "");
DECLARE_METRIC("nas_dl_attach", metric_nas_dl_attach, uint64_t, "");
DECLARE_METRIC("nas_ul_tau", metric_nas_ul_tau, uint64_t, "");
DECLARE_METRIC("nas_dl_tau", metric_nas_dl_tau, uint64_t, "");
DECLARE_METRIC("nas_ul_service_request", metric_nas_ul_service_request, uint64_t, "");
DECLARE_METRIC("nas_dl_service_request", metric_nas_dl_service_request, uint64_t, "");
DECLARE_METRIC("nas_dl_service_reject", metric_nas_dl_service_reject, uint64_t, "");
DECLARE_METRIC("nas_ul_identity", metric_nas_ul_identity, uint64_t, "");
DECLARE_METRIC("nas_dl_identity", metric_nas_dl_identity, uint64_t, "");
DECLARE_METRIC("nas_ul_authentication", metric_nas_ul_authentication, uint64_t, "");
DECLARE_METRIC("nas_dl_authentication", metric_nas_dl_authentication, uint64_t, "");
DECLARE_METRIC("nas_ul_security_mode", metric_nas_ul_security_mode, uint64_t, "");
DECLARE_METRIC("nas_dl_security_mode", metric_nas_dl_security_mode, uint64_t, "");
DECLARE_METRIC("nas_ul_detach", metric_nas_ul_detach, uint64_t, "");
DECLARE_METRIC("nas_dl_detach", metric_nas_dl_detach, uint64_t, "");
DECLARE_METRIC("nas_ul_emm_status", metric_nas_ul_emm_status, uint64_t, "");
DECLARE_METRIC("nas_dl_emm_status", metric_nas_dl_emm_status, uint64_t, "");
DECLARE_METRIC("nas_ul_esm_information", metric_nas_ul_esm_information, uint64_t, "");
DECLARE_METRIC("nas_dl_esm_information", metric_nas_dl_esm_information, uint64_t, "");
DECLARE_METRIC("nas_ul_pdn_connectivity", metric_nas_ul_pdn_connectivity, uint64_t, "");
DECLARE_METRIC("nas_dl_pdn_connectivity", metric_nas_dl_pdn_connectivity, uint64_t, "");
DECLARE_METRIC("nas_ul_pdn_disconnect", metric_nas_ul_pdn_disconnect, uint64_t, "");
DECLARE_METRIC("nas_dl_pdn_disconnect", metric_nas_dl_pdn_disconnect, uint64_t, "");
DECLARE_METRIC("nas_ul_default_bearer", metric_nas_ul_default_bearer, uint64_t, "");
DECLARE_METRIC("nas_dl_default_bearer", metric_nas_dl_default_bearer, uint64_t, "");
DECLARE_METRIC("nas_ul_dedicated_bearer", metric_nas_ul_dedicated_bearer, uint64_t, "");
DECLARE_METRIC("nas_dl_dedicated_bearer", metric_nas_dl_dedicated_bearer, uint64_t, "");
DECLARE_METRIC("nas_ul_modify_bearer", metric_nas_ul_modify_bearer, uint64_t, "");
DECLARE_METRIC("nas_dl_modify_bearer", metric_nas_dl_modify_bearer, uint64_t, "");
DECLARE_METRIC("nas_ul_deactivate_bearer", metric_nas_ul_deactivate_bearer, uint64_t, "");
DECLARE_METRIC("nas_dl_deactivate_bearer", metric_nas_dl_deactivate_bearer, uint64_t, "");
DECLARE_METRIC("nas_ul_bearer_resource", metric_nas_ul_bearer_resource, uint64_t, "");
DECLARE_METRIC("nas_dl_bearer_resource", metric_nas_dl_bearer_resource, uint64_t, "");
DECLARE_METRIC("nas_ul_generic_transport", metric_nas_ul_generic_transport, uint64_t, "");
DECLARE_METRIC("nas_dl_generic_transport", metric_nas_dl_generic_transport, uint64_t, "");
DECLARE_METRIC("nas_dl_cs_service_notification", metric_nas_dl_cs_service_notification, uint64_t, "");
DECLARE_METRIC("nas_ul_other", metric_nas_ul_other, uint64_t, "");
DECLARE_METRIC("nas_dl_other", metric_nas_dl_other, uint64_t, "");
DECLARE_METRIC("nas_ul_unknown", metric_nas_ul_unknown, uint64_t, "");
DECLARE_METRIC("nas_dl_unknown", metric_nas_dl_unknown, uint64_t, "");
DECLARE_METRIC_SET("s1ap_container",
                   mset_s1ap_container,
                   metric_s1ap_status,
                   metric_s1ap_status_code,
                   metric_nas_ul_msgs,
                   metric_nas_ul_fail,
                   metric_nas_dl_msgs,
                   metric_nas_dl_drop,
                   metric_nas_ul_bytes,
                   metric_nas_dl_bytes,
                   metric_nas_ul_transport_initial_ue,
                   metric_nas_ul_transport_ul_nas,
                   metric_nas_dl_transport_dl_nas,
                   metric_nas_ul_sec_hdr_plain,
                   metric_nas_ul_sec_hdr_integrity,
                   metric_nas_ul_sec_hdr_integrity_ciphered,
                   metric_nas_ul_sec_hdr_integrity_new_ctx,
                   metric_nas_ul_sec_hdr_integrity_ciphered_new_ctx,
                   metric_nas_ul_sec_hdr_service_request,
                   metric_nas_ul_sec_hdr_unknown,
                   metric_nas_dl_sec_hdr_plain,
                   metric_nas_dl_sec_hdr_integrity,
                   metric_nas_dl_sec_hdr_integrity_ciphered,
                   metric_nas_dl_sec_hdr_integrity_new_ctx,
                   metric_nas_dl_sec_hdr_integrity_ciphered_new_ctx,
                   metric_nas_dl_sec_hdr_service_request,
                   metric_nas_dl_sec_hdr_unknown,
                   metric_nas_ul_pd_emm,
                   metric_nas_ul_pd_esm,
                   metric_nas_ul_pd_other,
                   metric_nas_dl_pd_emm,
                   metric_nas_dl_pd_esm,
                   metric_nas_dl_pd_other,
                   metric_nas_ul_pd_last,
                   metric_nas_dl_pd_last,
                   metric_nas_ul_short_pdu,
                   metric_nas_dl_short_pdu,
                   metric_nas_ul_parse_fail,
                   metric_nas_dl_parse_fail,
                   metric_nas_ul_attach,
                   metric_nas_dl_attach,
                   metric_nas_ul_tau,
                   metric_nas_dl_tau,
                   metric_nas_ul_service_request,
                   metric_nas_dl_service_request,
                   metric_nas_dl_service_reject,
                   metric_nas_ul_identity,
                   metric_nas_dl_identity,
                   metric_nas_ul_authentication,
                   metric_nas_dl_authentication,
                   metric_nas_ul_security_mode,
                   metric_nas_dl_security_mode,
                   metric_nas_ul_detach,
                   metric_nas_dl_detach,
                   metric_nas_ul_emm_status,
                   metric_nas_dl_emm_status,
                   metric_nas_ul_esm_information,
                   metric_nas_dl_esm_information,
                   metric_nas_ul_pdn_connectivity,
                   metric_nas_dl_pdn_connectivity,
                   metric_nas_ul_pdn_disconnect,
                   metric_nas_dl_pdn_disconnect,
                   metric_nas_ul_default_bearer,
                   metric_nas_dl_default_bearer,
                   metric_nas_ul_dedicated_bearer,
                   metric_nas_dl_dedicated_bearer,
                   metric_nas_ul_modify_bearer,
                   metric_nas_dl_modify_bearer,
                   metric_nas_ul_deactivate_bearer,
                   metric_nas_dl_deactivate_bearer,
                   metric_nas_ul_bearer_resource,
                   metric_nas_dl_bearer_resource,
                   metric_nas_ul_generic_transport,
                   metric_nas_dl_generic_transport,
                   metric_nas_dl_cs_service_notification,
                   metric_nas_ul_other,
                   metric_nas_dl_other,
                   metric_nas_ul_unknown,
                   metric_nas_dl_unknown);

DECLARE_METRIC("rrc_total_ues", metric_rrc_total_ues, uint32_t, "");
DECLARE_METRIC("rrc_connected_ues", metric_rrc_connected_ues, uint32_t, "");
DECLARE_METRIC("rrc_con_req_rx", metric_rrc_con_req_rx_total, uint32_t, "");
DECLARE_METRIC("rrc_con_setup_tx", metric_rrc_con_setup_tx_total, uint32_t, "");
DECLARE_METRIC("rrc_con_setup_complete_rx", metric_rrc_con_setup_complete_rx_total, uint32_t, "");
DECLARE_METRIC("rrc_con_reject_tx", metric_rrc_con_reject_tx_total, uint32_t, "");
DECLARE_METRIC("rrc_con_reest_req_rx", metric_rrc_con_reest_req_rx_total, uint32_t, "");
DECLARE_METRIC("rrc_con_reest_tx", metric_rrc_con_reest_tx_total, uint32_t, "");
DECLARE_METRIC("rrc_con_reest_complete_rx", metric_rrc_con_reest_complete_rx_total, uint32_t, "");
DECLARE_METRIC("rrc_con_reest_reject_tx", metric_rrc_con_reest_reject_tx_total, uint32_t, "");
DECLARE_METRIC("rrc_con_reconf_tx", metric_rrc_con_reconf_tx_total, uint32_t, "");
DECLARE_METRIC("rrc_con_reconf_complete_rx", metric_rrc_con_reconf_complete_rx_total, uint32_t, "");
DECLARE_METRIC("rrc_con_release_tx", metric_rrc_con_release_tx_total, uint32_t, "");
DECLARE_METRIC("rrc_security_mode_command_tx", metric_rrc_security_mode_command_tx_total, uint32_t, "");
DECLARE_METRIC("rrc_security_mode_complete_rx", metric_rrc_security_mode_complete_rx_total, uint32_t, "");
DECLARE_METRIC("rrc_security_mode_failure_rx", metric_rrc_security_mode_failure_rx_total, uint32_t, "");
DECLARE_METRIC("rrc_ue_cap_enquiry_tx", metric_rrc_ue_cap_enquiry_tx_total, uint32_t, "");
DECLARE_METRIC("rrc_ue_cap_info_rx", metric_rrc_ue_cap_info_rx_total, uint32_t, "");
DECLARE_METRIC("rrc_ue_info_req_tx", metric_rrc_ue_info_req_tx_total, uint32_t, "");
DECLARE_METRIC("rrc_ue_info_resp_rx", metric_rrc_ue_info_resp_rx_total, uint32_t, "");
DECLARE_METRIC("rrc_max_rlc_retx", metric_rrc_max_rlc_retx_total, uint32_t, "");
DECLARE_METRIC("rrc_protocol_fail", metric_rrc_protocol_fail_total, uint32_t, "");
DECLARE_METRIC("rrc_paging_requests_total", metric_rrc_paging_requests_total, uint32_t, "");
DECLARE_METRIC("rrc_paging_imsi", metric_rrc_paging_imsi, uint32_t, "");
DECLARE_METRIC("rrc_paging_tmsi", metric_rrc_paging_tmsi, uint32_t, "");
DECLARE_METRIC("rrc_paging_add_fail", metric_rrc_paging_add_fail, uint32_t, "");
DECLARE_METRIC("rrc_paging_pdu_tx", metric_rrc_paging_pdu_tx, uint32_t, "");
DECLARE_METRIC("rrc_paging_bytes_tx", metric_rrc_paging_bytes_tx, uint32_t, "");
DECLARE_METRIC("rrc_paging_identities_tx", metric_rrc_paging_identities_tx, uint32_t, "");
DECLARE_METRIC("rrc_pdcp_integrity_errors", metric_rrc_pdcp_integrity_errors, uint32_t, "");
DECLARE_METRIC("rrc_state_idle", metric_rrc_state_idle, uint32_t, "");
DECLARE_METRIC("rrc_state_wait_for_con_setup_complete", metric_rrc_state_wait_for_con_setup_complete, uint32_t, "");
DECLARE_METRIC("rrc_state_wait_for_con_reest_complete", metric_rrc_state_wait_for_con_reest_complete, uint32_t, "");
DECLARE_METRIC("rrc_state_wait_for_security_mode_complete", metric_rrc_state_wait_for_security_mode_complete, uint32_t, "");
DECLARE_METRIC("rrc_state_wait_for_ue_cap_info", metric_rrc_state_wait_for_ue_cap_info, uint32_t, "");
DECLARE_METRIC("rrc_state_wait_for_ue_cap_info_endc", metric_rrc_state_wait_for_ue_cap_info_endc, uint32_t, "");
DECLARE_METRIC("rrc_state_wait_for_con_reconf_complete", metric_rrc_state_wait_for_con_reconf_complete, uint32_t, "");
DECLARE_METRIC("rrc_state_reestablishment_complete", metric_rrc_state_reestablishment_complete, uint32_t, "");
DECLARE_METRIC("rrc_state_registered", metric_rrc_state_registered, uint32_t, "");
DECLARE_METRIC("rrc_state_release_request", metric_rrc_state_release_request, uint32_t, "");
DECLARE_METRIC_SET("rrc_container",
                   mset_rrc_container,
                   metric_rrc_total_ues,
                   metric_rrc_connected_ues,
                   metric_rrc_con_req_rx_total,
                   metric_rrc_con_setup_tx_total,
                   metric_rrc_con_setup_complete_rx_total,
                   metric_rrc_con_reject_tx_total,
                   metric_rrc_con_reest_req_rx_total,
                   metric_rrc_con_reest_tx_total,
                   metric_rrc_con_reest_complete_rx_total,
                   metric_rrc_con_reest_reject_tx_total,
                   metric_rrc_con_reconf_tx_total,
                   metric_rrc_con_reconf_complete_rx_total,
                   metric_rrc_con_release_tx_total,
                   metric_rrc_security_mode_command_tx_total,
                   metric_rrc_security_mode_complete_rx_total,
                   metric_rrc_security_mode_failure_rx_total,
                   metric_rrc_ue_cap_enquiry_tx_total,
                   metric_rrc_ue_cap_info_rx_total,
                   metric_rrc_ue_info_req_tx_total,
                   metric_rrc_ue_info_resp_rx_total,
                   metric_rrc_max_rlc_retx_total,
                   metric_rrc_protocol_fail_total,
                   metric_rrc_paging_requests_total,
                   metric_rrc_paging_imsi,
                   metric_rrc_paging_tmsi,
                   metric_rrc_paging_add_fail,
                   metric_rrc_paging_pdu_tx,
                   metric_rrc_paging_bytes_tx,
                   metric_rrc_paging_identities_tx,
                   metric_rrc_pdcp_integrity_errors,
                   metric_rrc_state_idle,
                   metric_rrc_state_wait_for_con_setup_complete,
                   metric_rrc_state_wait_for_con_reest_complete,
                   metric_rrc_state_wait_for_security_mode_complete,
                   metric_rrc_state_wait_for_ue_cap_info,
                   metric_rrc_state_wait_for_ue_cap_info_endc,
                   metric_rrc_state_wait_for_con_reconf_complete,
                   metric_rrc_state_reestablishment_complete,
                   metric_rrc_state_registered,
                   metric_rrc_state_release_request);

/// Metrics context.
using metric_context_t =
    srslog::build_context_type<metric_type_tag,
                               metric_enb_serial,
                               metric_timestamp_tag,
                               mset_s1ap_container,
                               mset_rrc_container,
                               mlist_cell>;

} // namespace

static const char* s1ap_status_to_string(srsenb::S1AP_STATUS_ENUM status)
{
  switch (status) {
    case srsenb::S1AP_ATTACHING:
      return "attaching";
    case srsenb::S1AP_READY:
      return "ready";
    case srsenb::S1AP_ERROR:
    default:
      return "error";
  }
}

static const char* rrc_state_to_string(srsenb::rrc_state_t state)
{
  switch (state) {
    case srsenb::RRC_STATE_IDLE:
      return "idle";
    case srsenb::RRC_STATE_WAIT_FOR_CON_SETUP_COMPLETE:
      return "wait_for_con_setup_complete";
    case srsenb::RRC_STATE_WAIT_FOR_CON_REEST_COMPLETE:
      return "wait_for_con_reest_complete";
    case srsenb::RRC_STATE_WAIT_FOR_SECURITY_MODE_COMPLETE:
      return "wait_for_security_mode_complete";
    case srsenb::RRC_STATE_WAIT_FOR_UE_CAP_INFO:
      return "wait_for_ue_cap_info";
    case srsenb::RRC_STATE_WAIT_FOR_UE_CAP_INFO_ENDC:
      return "wait_for_ue_cap_info_endc";
    case srsenb::RRC_STATE_WAIT_FOR_CON_RECONF_COMPLETE:
      return "wait_for_con_reconf_complete";
    case srsenb::RRC_STATE_REESTABLISHMENT_COMPLETE:
      return "reestablishment_complete";
    case srsenb::RRC_STATE_REGISTERED:
      return "registered";
    case srsenb::RRC_STATE_RELEASE_REQUEST:
      return "release_request";
    case srsenb::RRC_STATE_N_ITEMS:
    default:
      return "unknown";
  }
}

/// Fill the metrics for the i'th UE in the enb metrics struct.
static void fill_ue_metrics(mset_ue_container& ue, const enb_metrics_t& m, unsigned i)
{
  ue.write<metric_ue_rnti>(m.stack.mac.ues[i].rnti);
  ue.write<metric_dl_cqi>(std::max(0.1f, m.stack.mac.ues[i].dl_cqi));
  if (!std::isnan(m.phy[i].dl.mcs)) {
    ue.write<metric_dl_mcs>(m.phy[i].dl.mcs);
  }
  if (m.stack.mac.ues[i].tx_brate > 0 && m.stack.mac.ues[i].nof_tti > 0) {
    ue.write<metric_dl_bitrate>(
        std::max(0.1f, (float)m.stack.mac.ues[i].tx_brate / (m.stack.mac.ues[i].nof_tti * 0.001f)));
  }
  if (m.stack.mac.ues[i].tx_pkts > 0 && m.stack.mac.ues[i].tx_errors > 0) {
    ue.write<metric_dl_bler>((float)100 * m.stack.mac.ues[i].tx_errors / m.stack.mac.ues[i].tx_pkts);
  }
  if (!std::isnan(m.phy[i].ul.pusch_sinr)) {
    ue.write<metric_ul_snr>(m.phy[i].ul.pusch_sinr);
  }
  if (!std::isnan(m.phy[i].ul.pusch_rssi)) {
    ue.write<metric_ul_pusch_rssi>(m.phy[i].ul.pusch_rssi);
  }
  if (!std::isnan(m.phy[i].ul.pucch_rssi)) {
    ue.write<metric_ul_pucch_rssi>(m.phy[i].ul.pucch_rssi);
  }
  if (!std::isnan(m.phy[i].ul.pucch_ni)) {
    ue.write<metric_ul_pucch_ni>(m.phy[i].ul.pucch_ni);
  }
  ue.write<metric_ul_pusch_tpc>(m.phy[i].ul.pusch_tpc);
  ue.write<metric_ul_pucch_tpc>(m.phy[i].dl.pucch_tpc);
  if (!std::isnan(m.stack.mac.ues[i].dl_cqi_offset)) {
    ue.write<metric_dl_cqi_offset>(m.stack.mac.ues[i].dl_cqi_offset);
  }
  if (!std::isnan(m.stack.mac.ues[i].ul_snr_offset)) {
    ue.write<metric_ul_snr_offset>(m.stack.mac.ues[i].ul_snr_offset);
  }
  if (!std::isnan(m.phy[i].ul.mcs)) {
    ue.write<metric_ul_mcs>(m.phy[i].ul.mcs);
  }
  if (m.stack.mac.ues[i].rx_brate > 0 && m.stack.mac.ues[i].nof_tti > 0) {
    ue.write<metric_ul_bitrate>((float)m.stack.mac.ues[i].rx_brate / (m.stack.mac.ues[i].nof_tti * 0.001f));
  }
  if (m.stack.mac.ues[i].rx_pkts > 0 && m.stack.mac.ues[i].rx_errors > 0) {
    ue.write<metric_ul_bler>(std::max(0.1f, (float)100 * m.stack.mac.ues[i].rx_errors / m.stack.mac.ues[i].rx_pkts));
  }
  ue.write<metric_ul_phr>(m.stack.mac.ues[i].phr);
  ue.write<metric_bsr>(m.stack.mac.ues[i].ul_buffer);
  ue.write<metric_rrc_state_str>(rrc_state_to_string(m.stack.rrc.ues[i].state));
  ue.write<metric_rrc_state>(m.stack.rrc.ues[i].state_code);
  ue.write<metric_rrc_drb_count>(m.stack.rrc.ues[i].drb_qci_map.size());
  ue.write<metric_rrc_nof_cells>(m.stack.rrc.ues[i].nof_cells);
  ue.write<metric_rrc_is_allocated>(m.stack.rrc.ues[i].is_allocated);
  ue.write<metric_rrc_sr_res_present>(m.stack.rrc.ues[i].sr_res_present);
  ue.write<metric_rrc_n_pucch_cs_present>(m.stack.rrc.ues[i].n_pucch_cs_present);
  ue.write<metric_rrc_is_csfb>(m.stack.rrc.ues[i].is_csfb);
  ue.write<metric_rrc_connect_notified>(m.stack.rrc.ues[i].connect_notified);
  ue.write<metric_rrc_rlf_cnt>(m.stack.rrc.ues[i].rlf_cnt);
  ue.write<metric_rrc_rlf_info_pending>(m.stack.rrc.ues[i].rlf_info_pending);
  ue.write<metric_rrc_consecutive_kos_dl>(m.stack.rrc.ues[i].consecutive_kos_dl);
  ue.write<metric_rrc_consecutive_kos_ul>(m.stack.rrc.ues[i].consecutive_kos_ul);
  ue.write<metric_rrc_has_tmsi>(m.stack.rrc.ues[i].has_tmsi);
  ue.write<metric_rrc_m_tmsi>(m.stack.rrc.ues[i].m_tmsi);
  ue.write<metric_rrc_mmec>(m.stack.rrc.ues[i].mmec);
  ue.write<metric_rrc_establishment_cause>(m.stack.rrc.ues[i].establishment_cause);
  ue.write<metric_rrc_transaction_id>(m.stack.rrc.ues[i].transaction_id);
  ue.write<metric_rrc_activity_timer_running>(m.stack.rrc.ues[i].activity_timer_running);
  ue.write<metric_rrc_activity_timer_elapsed>(m.stack.rrc.ues[i].activity_timer_elapsed);
  ue.write<metric_rrc_activity_timer_duration>(m.stack.rrc.ues[i].activity_timer_duration);
  ue.write<metric_rrc_phy_dl_rlf_timer_running>(m.stack.rrc.ues[i].phy_dl_rlf_timer_running);
  ue.write<metric_rrc_phy_dl_rlf_timer_elapsed>(m.stack.rrc.ues[i].phy_dl_rlf_timer_elapsed);
  ue.write<metric_rrc_phy_dl_rlf_timer_duration>(m.stack.rrc.ues[i].phy_dl_rlf_timer_duration);
  ue.write<metric_rrc_phy_ul_rlf_timer_running>(m.stack.rrc.ues[i].phy_ul_rlf_timer_running);
  ue.write<metric_rrc_phy_ul_rlf_timer_elapsed>(m.stack.rrc.ues[i].phy_ul_rlf_timer_elapsed);
  ue.write<metric_rrc_phy_ul_rlf_timer_duration>(m.stack.rrc.ues[i].phy_ul_rlf_timer_duration);
  ue.write<metric_rrc_rlc_rlf_timer_running>(m.stack.rrc.ues[i].rlc_rlf_timer_running);
  ue.write<metric_rrc_rlc_rlf_timer_elapsed>(m.stack.rrc.ues[i].rlc_rlf_timer_elapsed);
  ue.write<metric_rrc_rlc_rlf_timer_duration>(m.stack.rrc.ues[i].rlc_rlf_timer_duration);
  ue.write<metric_rrc_last_ul_msg_bytes>(m.stack.rrc.ues[i].last_ul_msg_bytes);
  ue.write<metric_rrc_eutra_capabilities_unpacked>(m.stack.rrc.ues[i].eutra_capabilities_unpacked);
  ue.write<metric_rrc_release_cause>(m.stack.rrc.ues[i].rrc_release_cause);
  ue.write<metric_rrc_con_req_rx>(m.stack.rrc.ues[i].rrc_con_req_rx);
  ue.write<metric_rrc_con_setup_tx>(m.stack.rrc.ues[i].rrc_con_setup_tx);
  ue.write<metric_rrc_con_setup_complete_rx>(m.stack.rrc.ues[i].rrc_con_setup_complete_rx);
  ue.write<metric_rrc_con_reject_tx>(m.stack.rrc.ues[i].rrc_con_reject_tx);
  ue.write<metric_rrc_con_reest_req_rx>(m.stack.rrc.ues[i].rrc_con_reest_req_rx);
  ue.write<metric_rrc_con_reest_tx>(m.stack.rrc.ues[i].rrc_con_reest_tx);
  ue.write<metric_rrc_con_reest_complete_rx>(m.stack.rrc.ues[i].rrc_con_reest_complete_rx);
  ue.write<metric_rrc_con_reest_reject_tx>(m.stack.rrc.ues[i].rrc_con_reest_reject_tx);
  ue.write<metric_rrc_con_reconf_tx>(m.stack.rrc.ues[i].rrc_con_reconf_tx);
  ue.write<metric_rrc_con_reconf_complete_rx>(m.stack.rrc.ues[i].rrc_con_reconf_complete_rx);
  ue.write<metric_rrc_con_release_tx>(m.stack.rrc.ues[i].rrc_con_release_tx);
  ue.write<metric_rrc_security_mode_command_tx>(m.stack.rrc.ues[i].rrc_security_mode_command_tx);
  ue.write<metric_rrc_security_mode_complete_rx>(m.stack.rrc.ues[i].rrc_security_mode_complete_rx);
  ue.write<metric_rrc_security_mode_failure_rx>(m.stack.rrc.ues[i].rrc_security_mode_failure_rx);
  ue.write<metric_rrc_ue_cap_enquiry_tx>(m.stack.rrc.ues[i].rrc_ue_cap_enquiry_tx);
  ue.write<metric_rrc_ue_cap_info_rx>(m.stack.rrc.ues[i].rrc_ue_cap_info_rx);
  ue.write<metric_rrc_ue_info_req_tx>(m.stack.rrc.ues[i].rrc_ue_info_req_tx);
  ue.write<metric_rrc_ue_info_resp_rx>(m.stack.rrc.ues[i].rrc_ue_info_resp_rx);
  ue.write<metric_rrc_max_rlc_retx>(m.stack.rrc.ues[i].rrc_max_rlc_retx);
  ue.write<metric_rrc_protocol_fail>(m.stack.rrc.ues[i].rrc_protocol_fail);

  // For each data bearer of this UE...
  auto& bearer_list = ue.get<mlist_bearers>();
  for (const auto& drb : m.stack.rrc.ues[i].drb_qci_map) {
    bearer_list.emplace_back();
    auto& bearer_container = bearer_list.back();
    bearer_container.write<metric_bearer_id>(drb.first);
    bearer_container.write<metric_qci>(drb.second);
    // RLC bearer metrics.
    if (drb.first >= SRSRAN_N_RADIO_BEARERS) {
      continue;
    }
    const auto& rlc_bearer  = m.stack.rlc.ues[i].bearer;
    const auto& pdcp_bearer = m.stack.pdcp.ues[i].bearer;
    bearer_container.write<metric_dl_total_bytes>(pdcp_bearer[drb.first].num_tx_acked_bytes);
    bearer_container.write<metric_ul_total_bytes>(pdcp_bearer[drb.first].num_rx_pdu_bytes);
    bearer_container.write<metric_dl_latency>(pdcp_bearer[drb.first].tx_notification_latency_ms / 1e3);
    bearer_container.write<metric_ul_latency>(rlc_bearer[drb.first].rx_latency_ms / 1e3);
    bearer_container.write<metric_dl_buffered_bytes>(pdcp_bearer[drb.first].num_tx_buffered_pdus_bytes);
    bearer_container.write<metric_ul_buffered_bytes>(rlc_bearer[drb.first].rx_buffered_bytes);
  }
}

/// Returns the current time in seconds with ms precision since UNIX epoch.
static double get_time_stamp()
{
  auto tp = std::chrono::system_clock::now().time_since_epoch();
  return std::chrono::duration_cast<std::chrono::milliseconds>(tp).count() * 1e-3;
}

/// Returns false if the input index is out of bounds in the metrics struct.
static bool has_valid_metric_ranges(const enb_metrics_t& m, unsigned index)
{
  if (index >= m.phy.size()) {
    return false;
  }
  if (index >= m.stack.mac.ues.size()) {
    return false;
  }
  if (index >= m.stack.rrc.ues.size()) {
    return false;
  }
  if (index >= m.stack.rlc.ues.size()) {
    return false;
  }
  if (index >= m.stack.pdcp.ues.size()) {
    return false;
  }

  return true;
}

void metrics_json::set_metrics(const enb_metrics_t& m, const uint32_t period_usec)
{
  if (!enb) {
    return;
  }
  if (m.stack.mac.cc_info.empty()) {
    return;
  }

  metric_context_t ctx("JSON Metrics");

  // Fill root object.
  ctx.write<metric_type_tag>("enb_metrics");
  if (!enb_serial.empty()) {
    ctx.write<metric_enb_serial>(enb_serial);
  }
  auto& cell_list = ctx.get<mlist_cell>();
  cell_list.resize(m.stack.mac.cc_info.size());

  // For each cell...
  for (unsigned cc_idx = 0, e = cell_list.size(); cc_idx != e; ++cc_idx) {
    auto& cell = cell_list[cc_idx];
    cell.write<metric_carrier_id>(cc_idx);
    cell.write<metric_nof_rach>(m.stack.mac.cc_info[cc_idx].cc_rach_counter);
    cell.write<metric_pci>(m.stack.mac.cc_info[cc_idx].pci);

    // For each UE in this cell...
    for (unsigned i = 0; i != m.stack.rrc.ues.size(); ++i) {
      if (!has_valid_metric_ranges(m, i)) {
        continue;
      }

      // Only record UEs that belong to this cell.
      if (m.stack.mac.ues[i].cc_idx != cc_idx) {
        continue;
      }
      cell.get<mlist_ues>().emplace_back();
      fill_ue_metrics(cell.get<mlist_ues>().back(), m, i);
    }
  }

  // S1AP / NAS metrics.
  auto& s1ap = ctx.get<mset_s1ap_container>();
  s1ap.write<metric_s1ap_status_code>(static_cast<uint32_t>(m.stack.s1ap.status));
  s1ap.write<metric_s1ap_status>(s1ap_status_to_string(m.stack.s1ap.status));
  s1ap.write<metric_nas_ul_msgs>(m.stack.s1ap.nas_ul_msgs);
  s1ap.write<metric_nas_ul_fail>(m.stack.s1ap.nas_ul_fail);
  s1ap.write<metric_nas_dl_msgs>(m.stack.s1ap.nas_dl_msgs);
  s1ap.write<metric_nas_dl_drop>(m.stack.s1ap.nas_dl_drop);
  s1ap.write<metric_nas_ul_bytes>(m.stack.s1ap.nas_ul_bytes);
  s1ap.write<metric_nas_dl_bytes>(m.stack.s1ap.nas_dl_bytes);
  s1ap.write<metric_nas_ul_transport_initial_ue>(m.stack.s1ap.nas_ul_transport_initial_ue);
  s1ap.write<metric_nas_ul_transport_ul_nas>(m.stack.s1ap.nas_ul_transport_ul_nas);
  s1ap.write<metric_nas_dl_transport_dl_nas>(m.stack.s1ap.nas_dl_transport_dl_nas);
  s1ap.write<metric_nas_ul_sec_hdr_plain>(m.stack.s1ap.nas_ul_sec_hdr_plain);
  s1ap.write<metric_nas_ul_sec_hdr_integrity>(m.stack.s1ap.nas_ul_sec_hdr_integrity);
  s1ap.write<metric_nas_ul_sec_hdr_integrity_ciphered>(m.stack.s1ap.nas_ul_sec_hdr_integrity_ciphered);
  s1ap.write<metric_nas_ul_sec_hdr_integrity_new_ctx>(m.stack.s1ap.nas_ul_sec_hdr_integrity_new_ctx);
  s1ap.write<metric_nas_ul_sec_hdr_integrity_ciphered_new_ctx>(
      m.stack.s1ap.nas_ul_sec_hdr_integrity_ciphered_new_ctx);
  s1ap.write<metric_nas_ul_sec_hdr_service_request>(m.stack.s1ap.nas_ul_sec_hdr_service_request);
  s1ap.write<metric_nas_ul_sec_hdr_unknown>(m.stack.s1ap.nas_ul_sec_hdr_unknown);
  s1ap.write<metric_nas_dl_sec_hdr_plain>(m.stack.s1ap.nas_dl_sec_hdr_plain);
  s1ap.write<metric_nas_dl_sec_hdr_integrity>(m.stack.s1ap.nas_dl_sec_hdr_integrity);
  s1ap.write<metric_nas_dl_sec_hdr_integrity_ciphered>(m.stack.s1ap.nas_dl_sec_hdr_integrity_ciphered);
  s1ap.write<metric_nas_dl_sec_hdr_integrity_new_ctx>(m.stack.s1ap.nas_dl_sec_hdr_integrity_new_ctx);
  s1ap.write<metric_nas_dl_sec_hdr_integrity_ciphered_new_ctx>(
      m.stack.s1ap.nas_dl_sec_hdr_integrity_ciphered_new_ctx);
  s1ap.write<metric_nas_dl_sec_hdr_service_request>(m.stack.s1ap.nas_dl_sec_hdr_service_request);
  s1ap.write<metric_nas_dl_sec_hdr_unknown>(m.stack.s1ap.nas_dl_sec_hdr_unknown);
  s1ap.write<metric_nas_ul_pd_emm>(m.stack.s1ap.nas_ul_pd_emm);
  s1ap.write<metric_nas_ul_pd_esm>(m.stack.s1ap.nas_ul_pd_esm);
  s1ap.write<metric_nas_ul_pd_other>(m.stack.s1ap.nas_ul_pd_other);
  s1ap.write<metric_nas_dl_pd_emm>(m.stack.s1ap.nas_dl_pd_emm);
  s1ap.write<metric_nas_dl_pd_esm>(m.stack.s1ap.nas_dl_pd_esm);
  s1ap.write<metric_nas_dl_pd_other>(m.stack.s1ap.nas_dl_pd_other);
  s1ap.write<metric_nas_ul_pd_last>(m.stack.s1ap.nas_ul_pd_last);
  s1ap.write<metric_nas_dl_pd_last>(m.stack.s1ap.nas_dl_pd_last);
  s1ap.write<metric_nas_ul_short_pdu>(m.stack.s1ap.nas_ul_short_pdu);
  s1ap.write<metric_nas_dl_short_pdu>(m.stack.s1ap.nas_dl_short_pdu);
  s1ap.write<metric_nas_ul_parse_fail>(m.stack.s1ap.nas_ul_parse_fail);
  s1ap.write<metric_nas_dl_parse_fail>(m.stack.s1ap.nas_dl_parse_fail);
  s1ap.write<metric_nas_ul_attach>(m.stack.s1ap.nas_ul_attach);
  s1ap.write<metric_nas_dl_attach>(m.stack.s1ap.nas_dl_attach);
  s1ap.write<metric_nas_ul_tau>(m.stack.s1ap.nas_ul_tau);
  s1ap.write<metric_nas_dl_tau>(m.stack.s1ap.nas_dl_tau);
  s1ap.write<metric_nas_ul_service_request>(m.stack.s1ap.nas_ul_service_request);
  s1ap.write<metric_nas_dl_service_request>(m.stack.s1ap.nas_dl_service_request);
  s1ap.write<metric_nas_dl_service_reject>(m.stack.s1ap.nas_dl_service_reject);
  s1ap.write<metric_nas_ul_identity>(m.stack.s1ap.nas_ul_identity);
  s1ap.write<metric_nas_dl_identity>(m.stack.s1ap.nas_dl_identity);
  s1ap.write<metric_nas_ul_authentication>(m.stack.s1ap.nas_ul_authentication);
  s1ap.write<metric_nas_dl_authentication>(m.stack.s1ap.nas_dl_authentication);
  s1ap.write<metric_nas_ul_security_mode>(m.stack.s1ap.nas_ul_security_mode);
  s1ap.write<metric_nas_dl_security_mode>(m.stack.s1ap.nas_dl_security_mode);
  s1ap.write<metric_nas_ul_detach>(m.stack.s1ap.nas_ul_detach);
  s1ap.write<metric_nas_dl_detach>(m.stack.s1ap.nas_dl_detach);
  s1ap.write<metric_nas_ul_emm_status>(m.stack.s1ap.nas_ul_emm_status);
  s1ap.write<metric_nas_dl_emm_status>(m.stack.s1ap.nas_dl_emm_status);
  s1ap.write<metric_nas_ul_esm_information>(m.stack.s1ap.nas_ul_esm_information);
  s1ap.write<metric_nas_dl_esm_information>(m.stack.s1ap.nas_dl_esm_information);
  s1ap.write<metric_nas_ul_pdn_connectivity>(m.stack.s1ap.nas_ul_pdn_connectivity);
  s1ap.write<metric_nas_dl_pdn_connectivity>(m.stack.s1ap.nas_dl_pdn_connectivity);
  s1ap.write<metric_nas_ul_pdn_disconnect>(m.stack.s1ap.nas_ul_pdn_disconnect);
  s1ap.write<metric_nas_dl_pdn_disconnect>(m.stack.s1ap.nas_dl_pdn_disconnect);
  s1ap.write<metric_nas_ul_default_bearer>(m.stack.s1ap.nas_ul_default_bearer);
  s1ap.write<metric_nas_dl_default_bearer>(m.stack.s1ap.nas_dl_default_bearer);
  s1ap.write<metric_nas_ul_dedicated_bearer>(m.stack.s1ap.nas_ul_dedicated_bearer);
  s1ap.write<metric_nas_dl_dedicated_bearer>(m.stack.s1ap.nas_dl_dedicated_bearer);
  s1ap.write<metric_nas_ul_modify_bearer>(m.stack.s1ap.nas_ul_modify_bearer);
  s1ap.write<metric_nas_dl_modify_bearer>(m.stack.s1ap.nas_dl_modify_bearer);
  s1ap.write<metric_nas_ul_deactivate_bearer>(m.stack.s1ap.nas_ul_deactivate_bearer);
  s1ap.write<metric_nas_dl_deactivate_bearer>(m.stack.s1ap.nas_dl_deactivate_bearer);
  s1ap.write<metric_nas_ul_bearer_resource>(m.stack.s1ap.nas_ul_bearer_resource);
  s1ap.write<metric_nas_dl_bearer_resource>(m.stack.s1ap.nas_dl_bearer_resource);
  s1ap.write<metric_nas_ul_generic_transport>(m.stack.s1ap.nas_ul_generic_transport);
  s1ap.write<metric_nas_dl_generic_transport>(m.stack.s1ap.nas_dl_generic_transport);
  s1ap.write<metric_nas_dl_cs_service_notification>(m.stack.s1ap.nas_dl_cs_service_notification);
  s1ap.write<metric_nas_ul_other>(m.stack.s1ap.nas_ul_other);
  s1ap.write<metric_nas_dl_other>(m.stack.s1ap.nas_dl_other);
  s1ap.write<metric_nas_ul_unknown>(m.stack.s1ap.nas_ul_unknown);
  s1ap.write<metric_nas_dl_unknown>(m.stack.s1ap.nas_dl_unknown);

  // RRC aggregate metrics.
  uint32_t rrc_state_counts[srsenb::RRC_STATE_N_ITEMS] = {};
  for (const auto& ue_rrc : m.stack.rrc.ues) {
    const auto idx = static_cast<unsigned>(ue_rrc.state);
    if (idx < srsenb::RRC_STATE_N_ITEMS) {
      ++rrc_state_counts[idx];
    }
  }
  auto& rrc = ctx.get<mset_rrc_container>();
  rrc.write<metric_rrc_total_ues>(m.stack.rrc.total_ues);
  rrc.write<metric_rrc_connected_ues>(m.stack.rrc.connected_ues);
  rrc.write<metric_rrc_con_req_rx_total>(m.stack.rrc.rrc_con_req_rx);
  rrc.write<metric_rrc_con_setup_tx_total>(m.stack.rrc.rrc_con_setup_tx);
  rrc.write<metric_rrc_con_setup_complete_rx_total>(m.stack.rrc.rrc_con_setup_complete_rx);
  rrc.write<metric_rrc_con_reject_tx_total>(m.stack.rrc.rrc_con_reject_tx);
  rrc.write<metric_rrc_con_reest_req_rx_total>(m.stack.rrc.rrc_con_reest_req_rx);
  rrc.write<metric_rrc_con_reest_tx_total>(m.stack.rrc.rrc_con_reest_tx);
  rrc.write<metric_rrc_con_reest_complete_rx_total>(m.stack.rrc.rrc_con_reest_complete_rx);
  rrc.write<metric_rrc_con_reest_reject_tx_total>(m.stack.rrc.rrc_con_reest_reject_tx);
  rrc.write<metric_rrc_con_reconf_tx_total>(m.stack.rrc.rrc_con_reconf_tx);
  rrc.write<metric_rrc_con_reconf_complete_rx_total>(m.stack.rrc.rrc_con_reconf_complete_rx);
  rrc.write<metric_rrc_con_release_tx_total>(m.stack.rrc.rrc_con_release_tx);
  rrc.write<metric_rrc_security_mode_command_tx_total>(m.stack.rrc.rrc_security_mode_command_tx);
  rrc.write<metric_rrc_security_mode_complete_rx_total>(m.stack.rrc.rrc_security_mode_complete_rx);
  rrc.write<metric_rrc_security_mode_failure_rx_total>(m.stack.rrc.rrc_security_mode_failure_rx);
  rrc.write<metric_rrc_ue_cap_enquiry_tx_total>(m.stack.rrc.rrc_ue_cap_enquiry_tx);
  rrc.write<metric_rrc_ue_cap_info_rx_total>(m.stack.rrc.rrc_ue_cap_info_rx);
  rrc.write<metric_rrc_ue_info_req_tx_total>(m.stack.rrc.rrc_ue_info_req_tx);
  rrc.write<metric_rrc_ue_info_resp_rx_total>(m.stack.rrc.rrc_ue_info_resp_rx);
  rrc.write<metric_rrc_max_rlc_retx_total>(m.stack.rrc.rrc_max_rlc_retx);
  rrc.write<metric_rrc_protocol_fail_total>(m.stack.rrc.rrc_protocol_fail);
  rrc.write<metric_rrc_paging_requests_total>(m.stack.rrc.paging_requests_total);
  rrc.write<metric_rrc_paging_imsi>(m.stack.rrc.paging_imsi);
  rrc.write<metric_rrc_paging_tmsi>(m.stack.rrc.paging_tmsi);
  rrc.write<metric_rrc_paging_add_fail>(m.stack.rrc.paging_add_fail);
  rrc.write<metric_rrc_paging_pdu_tx>(m.stack.rrc.paging_pdu_tx);
  rrc.write<metric_rrc_paging_bytes_tx>(m.stack.rrc.paging_bytes_tx);
  rrc.write<metric_rrc_paging_identities_tx>(m.stack.rrc.paging_identities_tx);
  rrc.write<metric_rrc_pdcp_integrity_errors>(m.stack.rrc.pdcp_integrity_errors);
  rrc.write<metric_rrc_state_idle>(rrc_state_counts[srsenb::RRC_STATE_IDLE]);
  rrc.write<metric_rrc_state_wait_for_con_setup_complete>(
      rrc_state_counts[srsenb::RRC_STATE_WAIT_FOR_CON_SETUP_COMPLETE]);
  rrc.write<metric_rrc_state_wait_for_con_reest_complete>(
      rrc_state_counts[srsenb::RRC_STATE_WAIT_FOR_CON_REEST_COMPLETE]);
  rrc.write<metric_rrc_state_wait_for_security_mode_complete>(
      rrc_state_counts[srsenb::RRC_STATE_WAIT_FOR_SECURITY_MODE_COMPLETE]);
  rrc.write<metric_rrc_state_wait_for_ue_cap_info>(rrc_state_counts[srsenb::RRC_STATE_WAIT_FOR_UE_CAP_INFO]);
  rrc.write<metric_rrc_state_wait_for_ue_cap_info_endc>(
      rrc_state_counts[srsenb::RRC_STATE_WAIT_FOR_UE_CAP_INFO_ENDC]);
  rrc.write<metric_rrc_state_wait_for_con_reconf_complete>(
      rrc_state_counts[srsenb::RRC_STATE_WAIT_FOR_CON_RECONF_COMPLETE]);
  rrc.write<metric_rrc_state_reestablishment_complete>(
      rrc_state_counts[srsenb::RRC_STATE_REESTABLISHMENT_COMPLETE]);
  rrc.write<metric_rrc_state_registered>(rrc_state_counts[srsenb::RRC_STATE_REGISTERED]);
  rrc.write<metric_rrc_state_release_request>(rrc_state_counts[srsenb::RRC_STATE_RELEASE_REQUEST]);

  // Log the context.
  ctx.write<metric_timestamp_tag>(get_time_stamp());
  log_c(ctx);
}
