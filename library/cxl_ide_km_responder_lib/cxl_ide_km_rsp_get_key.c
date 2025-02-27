/**
 *  Copyright Notice:
 *  Copyright 2021-2022 DMTF. All rights reserved.
 *  License: BSD 3-Clause License. For full text see link: https://github.com/DMTF/libspdm/blob/main/LICENSE.md
 **/

#include "hal/base.h"
#include "hal/library/memlib.h"
#include "hal/library/debuglib.h"
#include "library/spdm_requester_lib.h"
#include "library/spdm_transport_pcidoe_lib.h"
#include "library/cxl_ide_km_device_lib.h"

#pragma pack(1)
typedef struct {
    cxl_ide_km_header_t header;
    uint8_t reserved[2];
    uint8_t stream_id;
    uint8_t reserved2;
    uint8_t key_sub_stream;
    uint8_t port_index;
    cxl_ide_km_aes_256_gcm_key_buffer_t key_buffer;
} cxl_ide_km_get_key_ack_mine_t;
#pragma pack()

/**
 *  Process the IDE_KM request and return the response.
 *
 *  @param request       the IDE_KM request message, start from cxl_ide_km_header_t.
 *  @param request_size  size in bytes of request.
 *  @param response      the IDE_KM response message, start from cxl_ide_km_header_t.
 *  @param response_size size in bytes of response.
 *
 *  @retval LIBSPDM_STATUS_SUCCESS The request is processed and the response is returned.
 *  @return ERROR          The request is not processed.
 **/
libspdm_return_t cxl_ide_km_get_response_get_key (const void *pci_doe_context,
                                                  const void *spdm_context, const uint32_t *session_id,
                                                  const void *request, size_t request_size,
                                                  void *response, size_t *response_size)
{
    const cxl_ide_km_get_key_t *ide_km_request;
    cxl_ide_km_get_key_ack_mine_t *ide_km_response;
    libspdm_return_t status;

    ide_km_request = request;
    ide_km_response = response;
    if (request_size != sizeof(cxl_ide_km_get_key_t)) {
        return LIBSPDM_STATUS_INVALID_MSG_SIZE;
    }
    LIBSPDM_ASSERT (*response_size >= sizeof(cxl_ide_km_get_key_ack_mine_t));
    *response_size = sizeof(cxl_ide_km_get_key_ack_mine_t);

    libspdm_zero_mem (response, *response_size);
    ide_km_response->header.object_id = CXL_IDE_KM_OBJECT_ID_GET_KEY_ACK;

    ide_km_response->stream_id = ide_km_request->stream_id;
    ide_km_response->key_sub_stream = ide_km_request->key_sub_stream;
    ide_km_response->port_index = ide_km_request->port_index;

    status = cxl_ide_km_device_get_key (pci_doe_context, spdm_context, session_id,
                                        ide_km_request->stream_id,
                                        ide_km_request->key_sub_stream,
                                        ide_km_request->port_index,
                                        (void *)&ide_km_response->key_buffer
                                        );
    if (LIBSPDM_STATUS_IS_ERROR(status)) {
        return status;
    }

    return LIBSPDM_STATUS_SUCCESS;
}
