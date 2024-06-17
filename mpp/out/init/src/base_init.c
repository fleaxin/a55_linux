/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */
#include <linux/module.h>
#include "ot_type.h"
#include "ot_common.h"
#include "mod_ext.h"
#include "mm_ext.h"
#include "ot_base_mod_init.h"
#include "ot_debug.h"

static td_u32 g_vb_force_exit = 0;
module_param(g_vb_force_exit, uint, S_IRUGO);

EXPORT_SYMBOL(OT_LOG);
EXPORT_SYMBOL(cmpi_mmz_malloc);
EXPORT_SYMBOL(cmpi_mmz_malloc_fix_addr);
EXPORT_SYMBOL(cmpi_mmz_free);
EXPORT_SYMBOL(cmpi_mmz_malloc_nocache);
EXPORT_SYMBOL(cmpi_mmz_malloc_cached);
EXPORT_SYMBOL(cmpi_remap_cached);
EXPORT_SYMBOL(cmpi_remap_nocache);
EXPORT_SYMBOL(cmpi_unmap);
/*************************MOD********************/
EXPORT_SYMBOL(cmpi_get_module_name);
EXPORT_SYMBOL(cmpi_get_module_func_by_id);
EXPORT_SYMBOL(cmpi_stop_modules);
EXPORT_SYMBOL(cmpi_query_modules);
EXPORT_SYMBOL(cmpi_exit_modules);
EXPORT_SYMBOL(cmpi_init_modules);
EXPORT_SYMBOL(cmpi_register_module);
EXPORT_SYMBOL(cmpi_unregister_module);

static int __init base_mod_init(void)
{
    vb_set_force_exit(g_vb_force_exit);
    comm_init();

    return 0;
}
static void __exit base_mod_exit(void)
{
    comm_exit();
}

module_init(base_mod_init);
module_exit(base_mod_exit);

MODULE_LICENSE("Proprietary");

