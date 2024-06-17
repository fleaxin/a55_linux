/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#include "sample_dsp_main.h"

#ifdef __LITEOS__
int app_main(int argc, char *argv[])
{
#else
/*
 * to process abnormal case
 */
static td_void sample_svp_dsp_handle_sig(td_s32 signo)
{
    if (SIGINT == signo || SIGTERM == signo) {
        sample_svp_dsp_dilate_handle_sig();
    }
}
int main(int argc, char *argv[])
{
    struct sigaction sa;
    (td_void)memset_s(&sa, sizeof(struct sigaction), 0, sizeof(struct sigaction));
    sa.sa_handler = sample_svp_dsp_handle_sig;
    sa.sa_flags = 0;
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);
#endif
    sample_svp_dsp_dilate();
    (void)argc;
    (void)argv;
}
