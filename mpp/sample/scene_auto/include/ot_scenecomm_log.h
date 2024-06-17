/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef OT_SCENECOMM_LOG_H
#define OT_SCENECOMM_LOG_H

#include "ot_type.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* * \addtogroup     APPCOMM */
/* * @{ */ /* * <!-- [APPCOMM] */

/* * color log macro define */
#define NONE "\033[m"
#define RED "\033[0;32;31m"
#define GREEN "\033[0;32;32m"
#define BLUE "\033[0;32;34m"
#define YELLOW "\033[1;33m"

/* * product log level define */
#define OT_SCENELOG_LEVEL_FATAL 0
#define OT_SCENELOG_LEVEL_ERROR 1
#define OT_SCENELOG_LEVEL_WARNING 2
#define OT_SCENELOG_LEVEL_DEBUG 3

/* * product log level */
#ifdef CFG_DEBUG_LOG_ON
#define OT_SCENELOG_LEVEL OT_SCENELOG_LEVEL_DEBUG
#else
#define OT_SCENELOG_LEVEL OT_SCENELOG_LEVEL_ERROR
#endif

#ifdef __LITEOS__
#define ot_scene_log(level, module, fmt, args...) do {                                \
        if (level <= OT_SCENELOG_LEVEL) {                                             \
            printf(module "(%s-%d:%d): " fmt, __FUNCTION__, __LINE__, level, ##args); \
        }                                                                             \
    } while (0)
#else
/* * general product log macro define */
#define ot_scene_log(level, module, fmt, args...)  do {                                        \
        if (level <= OT_SCENELOG_LEVEL) {                                                      \
            fprintf(stdout, module "(%s-%d:%d): " fmt, __FUNCTION__, __LINE__, level, ##args); \
        }                                                                                      \
    } while (0)
#endif

/* * product module name, canbe redefined in module */
#ifdef OT_MODULE
#undef OT_MODULE
#endif
#define OT_MODULE ""

#ifdef CFG_DEBUG_LOG_ON
/* * unified log macro define */
#define scene_loge(fmt, args...) ot_scene_log(OT_SCENELOG_LEVEL_ERROR, OT_MODULE, fmt, ##args)
#define scene_logw(fmt, args...) ot_scene_log(OT_SCENELOG_LEVEL_WARNING, OT_MODULE, fmt, ##args)
#define scene_logd(fmt, args...) ot_scene_log(OT_SCENELOG_LEVEL_DEBUG, OT_MODULE, fmt, ##args)
#else
#define scene_loge(fmt, args...) ot_scene_log(OT_SCENELOG_LEVEL_ERROR, OT_MODULE, fmt, ##args)
#define scene_logw(fmt, args...)
#define scene_logd(fmt, args...)
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* End of #ifndef OT_SCENECOMM_LOG_H */
