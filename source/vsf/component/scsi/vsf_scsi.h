/*****************************************************************************
 *   Copyright(C)2009-2019 by VSF Team                                       *
 *                                                                           *
 *  Licensed under the Apache License, Version 2.0 (the "License");          *
 *  you may not use this file except in compliance with the License.         *
 *  You may obtain a copy of the License at                                  *
 *                                                                           *
 *     http://www.apache.org/licenses/LICENSE-2.0                            *
 *                                                                           *
 *  Unless required by applicable law or agreed to in writing, software      *
 *  distributed under the License is distributed on an "AS IS" BASIS,        *
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. *
 *  See the License for the specific language governing permissions and      *
 *  limitations under the License.                                           *
 *                                                                           *
 ****************************************************************************/

#ifndef __VSF_SCSI_H__
#define __VSF_SCSI_H__

/*============================ INCLUDES ======================================*/

#include "./vsf_scsi_cfg.h"

#if VSF_USE_SCSI == ENABLED

#if     defined(VSF_SCSI_IMPLEMENT)
#   undef VSF_SCSI_IMPLEMENT
#   define __PLOOC_CLASS_IMPLEMENT
#elif   defined(VSF_SCSI_INHERIT)
#   undef VSF_SCSI_INHERIT
#   define __PLOOC_CLASS_INHERIT
#endif

#include "utilities/ooc_class.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

declare_simple_class(vk_scsi_t)

enum scsi_sensekey_t {
    SCSI_SENSEKEY_NO_SENSE                      = 0,
    SCSI_SENSEKEY_RECOVERED_ERROR               = 1,
    SCSI_SENSEKEY_NOT_READY                     = 2,
    SCSI_SENSEKEY_MEDIUM_ERROR                  = 3,
    SCSI_SENSEKEY_HARDWARE_ERROR                = 4,
    SCSI_SENSEKEY_ILLEGAL_REQUEST               = 5,
    SCSI_SENSEKEY_UNIT_ATTENTION                = 6,
    SCSI_SENSEKEY_DATA_PROTECT                  = 7,
    SCSI_SENSEKEY_BLANK_CHECK                   = 8,
    SCSI_SENSEKEY_VENDOR_SPECIFIC               = 9,
    SCSI_SENSEKEY_COPY_ABORTED                  = 10,
    SCSI_SENSEKEY_ABORTED_COMMAND               = 11,
    SCSI_SENSEKEY_VOLUME_OVERFLOW               = 13,
    SCSI_SENSEKEY_MISCOMPARE                    = 14,
};
typedef enum scsi_sensekey_t scsi_sensekey_t;

enum scsi_asc_t {
    SCSI_ASC_NONE                               = 0x00,
    SCSI_ASC_PARAMETER_LIST_LENGTH_ERROR        = 0x1A,
    SCSI_ASC_INVALID_COMMAND                    = 0x20,
    SCSI_ASC_INVALID_FIELD_IN_COMMAND           = 0x24,
    SCSI_ASC_INVALID_FIELD_IN_PARAMETER_LIST    = 0x26,
    SCSI_ASC_MEDIUM_HAVE_CHANGED                = 0x28,
    SCSI_ASC_ADDRESS_OUT_OF_RANGE               = 0x21,
    SCSI_ASC_MEDIUM_NOT_PRESENT                 = 0x3A,
};
typedef enum scsi_asc_t scsi_asc_t;

enum scsi_group_code_t {
    SCSI_GROUPCODE6                             = 0x00,
    SCSI_GROUPCODE10_1                          = 0x20,
    SCSI_GROUPCODE10_2                          = 0x40,
    SCSI_GROUPCODE16                            = 0x80,
    SCSI_GROUPCODE12                            = 0xA0,
};
typedef enum scsi_group_code_t scsi_group_code_t;

enum scsi_cmd_code_t {
    SCSI_CMDCODE_TEST_UNIT_READY                = 0x00,
    SCSI_CMDCODE_REQUEST_SENSE                  = 0x03, // SCSI_GROUPCODE6
    SCSI_CMDCODE_READ_FORMAT_CAPACITIES         = 0x03, // SCSI_GROUPCODE10_1
    SCSI_CMDCODE_READ_TOC                       = 0x03, // SCSI_GROUPCODE10_2
    SCSI_CMDCODE_FORMAT_UNIT                    = 0x04,
    SCSI_CMDCODE_READ_CAPACITY                  = 0x05,
    SCSI_CMDCODE_READ                           = 0x08,
    SCSI_CMDCODE_WRITE                          = 0x0A,
    SCSI_CMDCODE_GET_EVENT_STATUS_NOTIFY        = 0x0A, // SCSI_GROUPCODE10_2
    SCSI_CMDCODE_VERIFY                         = 0x0F,
    SCSI_CMDCODE_INQUIRY                        = 0x12,
    SCSI_CMDCODE_MODE_SELECT                    = 0x15,
    SCSI_CMDCODE_MODE_SENSE                     = 0x1A,
    SCSI_CMDCODE_START_STOP_UNIT                = 0x1B,
    SCSI_CMDCODE_SEND_DIAGNOSTIC                = 0x1D,
    SCSI_CMDCODE_ALLOW_MEDIUM_REMOVAL           = 0x1E,
};
typedef enum scsi_cmd_code_t scsi_cmd_code_t;

struct i_scsi_drv_t {
    void (*init)(uintptr_t target, vsf_evt_t evt);
    void (*fini)(uintptr_t target, vsf_evt_t evt);
    bool (*buffer)(vk_scsi_t *pthis, uint8_t *cbd, vsf_mem_t *mem);
    void (*execute)(uintptr_t target, vsf_evt_t evt);
#if VSF_USE_SERVICE_VSFSTREAM == ENABLED
    void (*execute_stream)(uintptr_t target, vsf_evt_t evt);
#endif
};
typedef struct i_scsi_drv_t i_scsi_drv_t;


def_simple_class(vk_scsi_t) {
    public_member(
        const i_scsi_drv_t *drv;
        void *param;
    )

    protected_member(
        struct {
            uint8_t *cbd;
            vsf_mem_t mem;
#if VSF_USE_SERVICE_VSFSTREAM == ENABLED
            vsf_stream_t *stream;
#endif
        } args;
        struct {
            uint32_t reply_len;
            vsf_err_t errcode;
        } result;
    )
};

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ INCLUDES ======================================*/

#include "./driver/virtual_scsi/vsf_virtual_scsi.h"
#include "./driver/mal_scsi/vsf_mal_scsi.h"

/*============================ PROTOTYPES ====================================*/

extern vsf_err_t vk_scsi_init(vk_scsi_t *pthis);
extern vsf_err_t vk_scsi_fini(vk_scsi_t *pthis);
// used to get mem from driver, if supported
extern bool vk_scsi_prepare_buffer(vk_scsi_t *pthis, uint8_t *cbd, vsf_mem_t *mem);
extern vsf_err_t vk_scsi_execute(vk_scsi_t *pthis, uint8_t *cbd, vsf_mem_t *mem);
#if VSF_USE_SERVICE_VSFSTREAM == ENABLED
extern vsf_err_t vk_scsi_execute_stream(vk_scsi_t *pthis, uint8_t *cbd, vsf_stream_t *stream);
#endif

extern vsf_err_t vk_scsi_get_errcode(vk_scsi_t *pthis, uint32_t *reply_len);

#endif      // VSF_USE_SCSI
#endif      // __VSF_SCSI_H__
