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

/*============================ INCLUDES ======================================*/

#include "vsf.h"

/*============================ MACROS ========================================*/

#ifndef USRAPP_CFG_CDC_NUM
#   define USRAPP_CFG_CDC_NUM               6
#endif

#if USRAPP_CFG_CDC_NUM > 8
#   error not support
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/

// __INT_EP starts from USRAPP_CFG_CDC_NUM + 1
// __I_FUNC starts from 4(first 3 are manufactorer/device/serial strings)
#if USRAPP_CFG_USBD_SPEED == USB_SPEED_HIGH
#   define USRAPP_DESC_CDC_CFG(__N, __UNUSED)                                   \
            USB_DESC_CDC_UART_HS_IAD((__N) * 2, 4 + (__N), USRAPP_CFG_CDC_NUM + 1 + (__N), 1 + (__N), 1 + (__N))
#elif USRAPP_CFG_USBD_SPEED == USB_SPEED_FULL
#   define USRAPP_DESC_CDC_CFG(__N, __UNUSED)                                   \
            USB_DESC_CDC_UART_FS_IAD((__N) * 2, 4 + (__N), USRAPP_CFG_CDC_NUM + 1 + (__N), 1 + (__N), 1 + (__N))
#endif

#define USRAPP_DESC_CDC_STRING(__N, __UNUSED)                                   \
            .str_cdc[(__N)]             = {                                     \
                USB_DESC_STRING(14,                                             \
                    'V', 0, 'S', 0, 'F', 0, 'C', 0, 'D', 0, 'C', 0, '0' + (__N), 0\
                )                                                               \
            },
#define USRAPP_DESC_CDC_STRING_DECLARE(__N, __USBD_CONST)                       \
            VSF_USBD_DESC_STRING(0x0409, 4 + (__N), __USBD_CONST.str_cdc[(__N)], sizeof(__USBD_CONST.str_cdc[(__N)])),

#define USRAPP_CDC_INSTANCE(__N, __USBD)                                        \
    .cdc[(__N)]                 = {                                             \
        .param                  = {                                             \
            .ep = {                                                             \
                .notify         = USRAPP_CFG_CDC_NUM + 1 + (__N),               \
                .out            = 1 + (__N),                                    \
                .in             = 1 + (__N),                                    \
            },                                                                  \
            .line_coding        = {                                             \
                .bitrate        = 115200,                                       \
                .stop           = 0,                                            \
                .parity         = 0,                                            \
                .datalen        = 8,                                            \
            },                                                                  \
            .stream.tx.stream   = (vsf_stream_t *)&__USBD.cdc[(__N)].stream.tx, \
            .stream.rx.stream   = (vsf_stream_t *)&__USBD.cdc[(__N)].stream.rx, \
        },                                                                      \
        .stream             = {                                                 \
            .tx.op              = &vsf_mem_stream_op,                           \
            .tx.pchBuffer       = (uint8_t *)&__USBD.cdc[(__N)].stream.tx_buffer,\
            .tx.nSize           = sizeof(__USBD.cdc[(__N)].stream.tx_buffer),   \
            .tx.align           = USRAPP_CFG_STREAM_ALIGN,                      \
            .rx.op              = &vsf_mem_stream_op,                           \
            .rx.pchBuffer       = (uint8_t *)&__USBD.cdc[(__N)].stream.rx_buffer,\
            .rx.nSize           = sizeof(__USBD.cdc[(__N)].stream.rx_buffer),   \
            .rx.align           = USRAPP_CFG_STREAM_ALIGN,                      \
        },                                                                      \
    },                                                                          \
    .ifs[2 * (__N)].class_op        = &vk_usbd_cdcacm_control,                 \
    .ifs[2 * (__N)].class_param     = &__USBD.cdc[(__N)].param,                 \
    .ifs[2 * (__N) + 1].class_op    = &vk_usbd_cdcacm_data,                    \
    .ifs[2 * (__N) + 1].class_param = &__USBD.cdc[(__N)].param,

/*============================ TYPES =========================================*/

struct usrapp_const_t {
    struct {
#if USRAPP_CFG_DCD_TYPE == USRAPP_CFG_DCD_TYPE_DWCOTG
        vk_dwcotg_dcd_param_t dwcotg_param;
#elif USRAPP_CFG_DCD_TYPE == USRAPP_CFG_DCD_TYPE_MUSB_FDRC
        vk_musb_fdrc_dcd_param_t musb_fdrc_param;
#endif
        uint8_t dev_desc[18];
        uint8_t config_desc[9 + USRAPP_CFG_CDC_NUM * USB_DESC_CDC_ACM_IAD_LEN];
        uint8_t str_lanid[4];
        uint8_t str_vendor[20];
        uint8_t str_product[26];
        uint8_t str_cdc[USRAPP_CFG_CDC_NUM][16];
        vk_usbd_desc_t std_desc[5 + USRAPP_CFG_CDC_NUM];
    } usbd;
};
typedef struct usrapp_const_t usrapp_const_t;

struct usrapp_t {
    struct {
#if USRAPP_CFG_DCD_TYPE == USRAPP_CFG_DCD_TYPE_DWCOTG
        vk_dwcotg_dcd_t dwcotg_dcd;
#elif USRAPP_CFG_DCD_TYPE == USRAPP_CFG_DCD_TYPE_MUSB_FDRC
        vk_musb_fdrc_dcd_t musb_fdrc_dcd;
#endif
        struct {
            vk_usbd_cdcacm_t param;
#if VSF_USE_SERVICE_VSFSTREAM == ENABLED
            struct {
                vsf_mem_stream_t tx;
                vsf_mem_stream_t rx;
                uint8_t tx_buffer[4 * 1024];
                uint8_t rx_buffer[4 * 1024];
            } stream;
#elif VSF_USE_SERVICE_STREAM == ENABLED
#endif
        } cdc[USRAPP_CFG_CDC_NUM];

        vk_usbd_ifs_t ifs[2 * USRAPP_CFG_CDC_NUM];
        vk_usbd_cfg_t config[1];
        vk_usbd_dev_t dev;

        vsf_callback_timer_t connect_timer;
    } usbd;
};
typedef struct usrapp_t usrapp_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/

static const usrapp_const_t usrapp_const = {
    .usbd                       = {
#if USRAPP_CFG_DCD_TYPE == USRAPP_CFG_DCD_TYPE_DWCOTG
        .dwcotg_param           = {
            .op                 = &VSF_USB_DC0_IP,
            .speed              = USRAPP_CFG_USBD_SPEED,
        },
#elif USRAPP_CFG_DCD_TYPE == USRAPP_CFG_DCD_TYPE_MUSB_FDRC
        musb_fdrc_param         = {
            .op                 = &VSF_USB_DC0_IP,
        },
#endif
        .dev_desc               = {
            USB_DESC_DEV_IAD(64, APP_CFG_USBD_VID, APP_CFG_USBD_PID, 1, 2, 0, 1)
        },
        .config_desc            = {
            USB_DESC_CFG(sizeof(usrapp_const.usbd.config_desc), 2 * USRAPP_CFG_CDC_NUM, 1, 0, 0x80, 100)
            REPEAT_MACRO(USRAPP_CFG_CDC_NUM, USRAPP_DESC_CDC_CFG, NULL)
        },
        .str_lanid              = {
            USB_DESC_STRING(2, 0x09, 0x04)
        },
        .str_vendor             = {
            USB_DESC_STRING(18,
                'S', 0, 'i', 0, 'm', 0, 'o', 0, 'n', 0, 'Q', 0, 'i', 0, 'a', 0,
                'n', 0
            )
        },
        .str_product            = {
            USB_DESC_STRING(24,
                'V', 0, 'S', 0, 'F', 0, '-', 0, 'M', 0, 'u', 0, 'l', 0, 't', 0,
                'i', 0, 'C', 0, 'D', 0, 'C', 0
            )
        },
        REPEAT_MACRO(USRAPP_CFG_CDC_NUM, USRAPP_DESC_CDC_STRING, NULL)
        .std_desc               = {
            VSF_USBD_DESC_DEVICE(0, usrapp_const.usbd.dev_desc, sizeof(usrapp_const.usbd.dev_desc)),
            VSF_USBD_DESC_CONFIG(0, 0, usrapp_const.usbd.config_desc, sizeof(usrapp_const.usbd.config_desc)),
            VSF_USBD_DESC_STRING(0, 0, usrapp_const.usbd.str_lanid, sizeof(usrapp_const.usbd.str_lanid)),
            VSF_USBD_DESC_STRING(0x0409, 1, usrapp_const.usbd.str_vendor, sizeof(usrapp_const.usbd.str_vendor)),
            VSF_USBD_DESC_STRING(0x0409, 2, usrapp_const.usbd.str_product, sizeof(usrapp_const.usbd.str_product)),
            REPEAT_MACRO(USRAPP_CFG_CDC_NUM, USRAPP_DESC_CDC_STRING_DECLARE, usrapp_const.usbd)
        },
    },
};

static usrapp_t usrapp;
#if USRAPP_CFG_DCD_TYPE == USRAPP_CFG_DCD_TYPE_DWCOTG
VSF_USB_DC_FROM_DWCOTG_IP(0, usrapp.usbd.dwcotg_dcd, VSF_USB_DC0)
#elif USRAPP_CFG_DCD_TYPE == USRAPP_CFG_DCD_TYPE_MUSB_FDRC
VSF_USB_DC_FROM_MUSB_FDRC_IP(0, usrapp.usbd.musb_fdrc_dcd, VSF_USB_DC0)
#endif

static usrapp_t usrapp = {
    .usbd                       = {
#if USRAPP_CFG_DCD_TYPE == USRAPP_CFG_DCD_TYPE_DWCOTG
        .dwcotg_dcd.param       = &usrapp_const.usbd.dwcotg_param,
#elif USRAPP_CFG_DCD_TYPE == USRAPP_CFG_DCD_TYPE_MUSB_FDRC
        .musb_fdrc_dcd.param    = &usrapp_const.usbd.musb_fdrc_param,
#endif
        REPEAT_MACRO(USRAPP_CFG_CDC_NUM, USRAPP_CDC_INSTANCE, usrapp.usbd)

        .config[0].num_of_ifs   = dimof(usrapp.usbd.ifs),
        .config[0].ifs          = usrapp.usbd.ifs,

        .dev.num_of_config      = dimof(usrapp.usbd.config),
        .dev.config             = usrapp.usbd.config,
        .dev.num_of_desc        = dimof(usrapp_const.usbd.std_desc),
        .dev.desc               = (vk_usbd_desc_t *)usrapp_const.usbd.std_desc,

#if USRAPP_CFG_USBD_SPEED == USB_SPEED_HIGH
        .dev.speed              = USB_DC_SPEED_HIGH,
#elif USRAPP_CFG_USBD_SPEED == USB_SPEED_FULL
        .dev.speed              = USB_DC_SPEED_FULL,
#endif
        .dev.drv                = &VSF_USB_DC0,//&VSF_USB.DC[0],
    },
};

/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

static void usrapp_on_timer(vsf_callback_timer_t *timer)
{
    vk_usbd_connect(&usrapp.usbd.dev);
}

void main(void)
{
#if VSF_USE_TRACE == ENABLED
    vsf_trace_init(NULL);
    vsf_stdio_init();
#endif

    for (uint_fast8_t i = 0; i < USRAPP_CFG_CDC_NUM; i++) {
        vsf_stream_init(&usrapp.usbd.cdc[i].stream.tx.use_as__vsf_stream_t);
        vsf_stream_init(&usrapp.usbd.cdc[i].stream.rx.use_as__vsf_stream_t);
    }

    vk_usbd_init(&usrapp.usbd.dev);
    vk_usbd_disconnect(&usrapp.usbd.dev);
    usrapp.usbd.connect_timer.on_timer = usrapp_on_timer;
    vsf_callback_timer_add_ms(&usrapp.usbd.connect_timer, 200);
}

/* EOF */
