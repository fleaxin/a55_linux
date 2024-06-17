[notes]
1.Since the compilation of the sample depends on the third-party open source library, when you compile this sample, you must prepare the open source library in advance, 
so you need to enter the open_source\iniparser directory to execute the 'make' command, or enter the upper level directory to execute the 'make' command
2.At present,tuning is performed only in AVBR bitrate control mode. If the user approves the effect, it can be developed based on this sample.
3.Since the smart target cannot be obtained, the user needs to configure additional parameters of the ss_mpi_venc_send_svc_region interface.

[example]
./sample_svc_rate param/config_rate_auto_base_param.ini
