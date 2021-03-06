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

#include "../../vsf_linux_cfg.h"

#if VSF_USE_LINUX == ENABLED

#define VSF_LINUX_INHERIT
#include "../../vsf_linux.h"
#include <errno.h>

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

void vsf_linux_glibc_init(void)
{
    
}

int __sync_pend(vsf_sync_t *sem)
{
    while (1) {
        vsf_sync_reason_t reason = vsf_eda_sync_get_reason(sem, vsf_thread_wait());
        switch (reason) {
        case VSF_SYNC_TIMEOUT:      return -ETIMEDOUT;
        case VSF_SYNC_PENDING:      break;
        case VSF_SYNC_GET:          return 0;
        case VSF_SYNC_CANCEL:       return -EAGAIN;
        }
    }
}

#ifndef __WIN__
char * strdup(const char *str)
{
    char *newstr = malloc(strlen(str) + 1);
    if (newstr != NULL) {
        strcpy(newstr, str);
    }
    return newstr;
}
#endif

#endif      // VSF_USE_LINUX
