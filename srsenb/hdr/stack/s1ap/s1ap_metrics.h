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

#ifndef SRSENB_S1AP_METRICS_H
#define SRSENB_S1AP_METRICS_H

#include <cstdint>

namespace srsenb {

typedef enum {
  S1AP_ATTACHING = 0, // Attempting to create S1 connection
  S1AP_READY,         // S1 connected
  S1AP_ERROR          // Failure
} S1AP_STATUS_ENUM;

struct s1ap_metrics_t {
  S1AP_STATUS_ENUM status;
  uint64_t         nas_ul_msgs;
  uint64_t         nas_ul_fail;
  uint64_t         nas_dl_msgs;
  uint64_t         nas_dl_drop;
  uint64_t         nas_ul_bytes;
  uint64_t         nas_dl_bytes;
  uint64_t         nas_ul_transport_initial_ue;
  uint64_t         nas_ul_transport_ul_nas;
  uint64_t         nas_dl_transport_dl_nas;
  uint64_t         nas_ul_sec_hdr_plain;
  uint64_t         nas_ul_sec_hdr_integrity;
  uint64_t         nas_ul_sec_hdr_integrity_ciphered;
  uint64_t         nas_ul_sec_hdr_integrity_new_ctx;
  uint64_t         nas_ul_sec_hdr_integrity_ciphered_new_ctx;
  uint64_t         nas_ul_sec_hdr_service_request;
  uint64_t         nas_ul_sec_hdr_unknown;
  uint64_t         nas_dl_sec_hdr_plain;
  uint64_t         nas_dl_sec_hdr_integrity;
  uint64_t         nas_dl_sec_hdr_integrity_ciphered;
  uint64_t         nas_dl_sec_hdr_integrity_new_ctx;
  uint64_t         nas_dl_sec_hdr_integrity_ciphered_new_ctx;
  uint64_t         nas_dl_sec_hdr_service_request;
  uint64_t         nas_dl_sec_hdr_unknown;
  uint64_t         nas_ul_pd_emm;
  uint64_t         nas_ul_pd_esm;
  uint64_t         nas_ul_pd_other;
  uint64_t         nas_dl_pd_emm;
  uint64_t         nas_dl_pd_esm;
  uint64_t         nas_dl_pd_other;
  uint32_t         nas_ul_pd_last;
  uint32_t         nas_dl_pd_last;
  uint64_t         nas_ul_short_pdu;
  uint64_t         nas_dl_short_pdu;
  uint64_t         nas_ul_parse_fail;
  uint64_t         nas_dl_parse_fail;
  uint64_t         nas_ul_attach;
  uint64_t         nas_dl_attach;
  uint64_t         nas_ul_tau;
  uint64_t         nas_dl_tau;
  uint64_t         nas_ul_service_request;
  uint64_t         nas_dl_service_request;
  uint64_t         nas_dl_service_reject;
  uint64_t         nas_ul_identity;
  uint64_t         nas_dl_identity;
  uint64_t         nas_ul_authentication;
  uint64_t         nas_dl_authentication;
  uint64_t         nas_ul_security_mode;
  uint64_t         nas_dl_security_mode;
  uint64_t         nas_ul_detach;
  uint64_t         nas_dl_detach;
  uint64_t         nas_ul_emm_status;
  uint64_t         nas_dl_emm_status;
  uint64_t         nas_ul_esm_information;
  uint64_t         nas_dl_esm_information;
  uint64_t         nas_ul_pdn_connectivity;
  uint64_t         nas_dl_pdn_connectivity;
  uint64_t         nas_ul_pdn_disconnect;
  uint64_t         nas_dl_pdn_disconnect;
  uint64_t         nas_ul_default_bearer;
  uint64_t         nas_dl_default_bearer;
  uint64_t         nas_ul_dedicated_bearer;
  uint64_t         nas_dl_dedicated_bearer;
  uint64_t         nas_ul_modify_bearer;
  uint64_t         nas_dl_modify_bearer;
  uint64_t         nas_ul_deactivate_bearer;
  uint64_t         nas_dl_deactivate_bearer;
  uint64_t         nas_ul_bearer_resource;
  uint64_t         nas_dl_bearer_resource;
  uint64_t         nas_ul_generic_transport;
  uint64_t         nas_dl_generic_transport;
  uint64_t         nas_dl_cs_service_notification;
  uint64_t         nas_ul_other;
  uint64_t         nas_dl_other;
  uint64_t         nas_ul_unknown;
  uint64_t         nas_dl_unknown;
};

} // namespace srsenb

#endif // SRSENB_S1AP_METRICS_H
