#!/bin/sh
# This script can start/stop/restart slogd process.
# Copyright (c) Shenshu Technologies Co., Ltd. 2020. All rights reserved.

SLOG_DIR="/usr/slog"
SLOGDLOG_DIR="/var/log/npu/slog"
NPU_DIR="/var/log/npu"
CONF_DIR="/var/log/npu/conf"
CONF_FILE="/var/log/npu/conf/slog/slog.conf"
CURRENT_CONF_FILE="./slog.conf"
FOLDER_PERM=750
INVISIBLE_DIR_PERM=700
FILE_PERM=640
CFG_MAX_PATH_LENGTH=64
LOG_AGENT_MAX_PATH_LENGTH=255

LOG_AGENGT_PATH_KEY="logAgentFileDir"
LOG_WORKSPACE_PATH_KEY="logWorkspace"

configPath=$CURRENT_CONF_FILE
workSpacepath=$SLOG_DIR
logAgentFileDir=$SLOGDLOG_DIR

cd "$(dirname $0)"
checkConfDir() {
    # npu
    if [ ! -d $NPU_DIR ];then
        mkdir -p $NPU_DIR
        if [ ! -d "$NPU_DIR" ];then
            echo "[INFO] Reading slog.conf in the current directory."
            return 1
        fi
    fi
    chmod $FOLDER_PERM $NPU_DIR

    # slog self log
    mkdir -p $SLOGDLOG_DIR/slogd
    chmod $FOLDER_PERM $SLOGDLOG_DIR
    chmod $FOLDER_PERM $SLOGDLOG_DIR/slogd

    # conf
    mkdir -p $CONF_DIR/slog
    chmod $FOLDER_PERM $CONF_DIR
    chmod $FOLDER_PERM $CONF_DIR/slog
    #copy conf file to /var/
    if [ ! -f "$CONF_FILE" ];then
        if [ ! -f "$CURRENT_CONF_FILE" ];then
            echo "[WARNING] slog.conf is not found in the current directory. The default slogd configuration will be used."
            return 1
        else
            cp "$CURRENT_CONF_FILE" "$CONF_FILE"
            if [ $? -ne 0 ];then
                echo "[WARNING] Failed to copy slog.conf to $CONF_FILE. Permission denined."\
                     "The default slogd configuration will be used."
                return 1
            fi
            chmod $FILE_PERM "$CONF_FILE"
            configPath=$CONF_FILE
        fi
    else
        chmod $FILE_PERM "$CONF_FILE"
        configPath=$CONF_FILE
    fi
}

checkSlogDir(){
    makeWorkSpaceFile
    if [ $? -ne 0 ];then return 1; fi
    if [ ! -d "$workSpacepath" ];then
        mkdir -p "$workSpacepath"
        if [ ! -d "$workSpacepath" ];then
            local user=$(whoami)
            echo "[ERROR] Failed to create $workSpacepath. Permission denied for user:" $user
            return 1
        fi
        chmod $FOLDER_PERM "$workSpacepath"
    else
        if [ -f "$workSpacepath/slogd.pid" ];then
            rm "$workSpacepath/slogd.pid"
            if [ $? -ne 0 ];then
                echo "[ERROR] Failed to remove $workSpacepath/slogd.pid. Permission denined for user:" $user\
                     ". Please remove it manually."
                return 1
            fi
        fi
    fi
    return 0
}

makeConfFile(){
    local pathConf=$(sed 's/\r//g' $configPath | grep $LOG_AGENGT_PATH_KEY)
    if [ $? -ne 0 ];then return 1;fi
    # ignore comment
    local path=${pathConf%#*}
    # get path value & trim space
    local rawPath=$(echo "${path#*=}" | sed "s/^\s*//g" | sed "s/\s*$//g")
    # ignore string after space, path should only relative to "/"
    path="/"${rawPath%% *}
    if [ -z "$path" ];then
        echo "[ERROR] Failed to obtain slog configuration from $configPath."
        return 1
    fi
    # because add '/', so length is added by 1
    tmplen=$(($LOG_AGENT_MAX_PATH_LENGTH+1))
    # check length
    if [ ${#path} -gt $tmplen ]
    then
        echo "[WARNING] The $LOG_AGENGT_PATH_KEY length is ${#path}, which exceeds $LOG_AGENT_MAX_PATH_LENGTH. Please check slog.conf."
        echo "[WARNING] The default slogd configuration will be used."
        return 1
    fi
    # replace "//" with '/'
    realpath=$(echo $path | sed 's/\/\//\//g')
    if [ ${#realpath} -gt 1 ];then
        logAgentFileDir=$realpath
    fi
    mkdir -p "$logAgentFileDir"
    if [ ! -d "$logAgentFileDir" ];then
        echo "[WARNING] Failed to create the log directory \"$logAgentFileDir\", or permission denied."
        echo "[WARNING] The default slogd configuration will be used."
        return 1
    fi
    chmod $FOLDER_PERM "$logAgentFileDir"
}

makeWorkSpaceFile(){
    local pathConf=$(sed 's/\r//g' $configPath | grep $LOG_WORKSPACE_PATH_KEY)
    if [ $? -ne 0 ];then return 1; fi
    # ignore comment
    local path=${pathConf%#*}
    # get path value & trim space
    local rawPath=$(echo "${path#*=}" | sed "s/^\s*//g" | sed "s/\s*$//g")
    # ignore string after space. The path should only be relative to "/"
    path="/"${rawPath%% *}
    if [ -z "$path" ];then
        echo "[ERROR] Failed to obtain slog configuration from $configPath."
        return 1
    fi
    # because add '/', so length is added by 1
    tmplen=$(($CFG_MAX_PATH_LENGTH+1))
    # check length
    if [ ${#path} -gt $tmplen ]
    then
        echo "[ERROR] The $LOG_WORKSPACE_PATH_KEY length is ${#path}, which exceeds $CFG_MAX_PATH_LENGTH. Please check the slog.conf."
        return 1
    fi
    # replace "//" to '/'
    realpath=$(echo $path | sed 's/\/\//\//g')
    if [ ${#realpath} -gt 1 ];then
        workSpacepath=$realpath
    fi
    mkdir -p "$workSpacepath"
    if [ ! -d "$workSpacepath" ];then
        echo "[ERROR] Failed to create the log directory \"$workSpacepath\", or permission denied."
        return 1
    fi
    chmod $FOLDER_PERM "$workSpacepath"
    return 0
}

checkConfFile(){
    if [ ! -f "$configPath" ];then
        echo "[WARNING] slog.conf is not found. The default slogd configuration will be used."
    else
        makeConfFile
    fi
    return 0
}

checkFileStatus(){
    local dir=$(pwd)
    checkConfFile
    if [ $? -ne 0 ];then
        return 1
    fi
    checkSlogDir
    if [ $? -ne 0 ];then
        return 1
    fi
    if [ ! -f "./slogd" ];then
        echo "[ERROR] slogd is not found in "$dir". Please place this script in the same directory of slogd."
        return 1
    fi
    # current dir is **/obj64/sample/npu/slog, so files should be in **/lib64/share
    local ldPath="$dir/../../../../lib64/share"
    export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$ldPath
    return 0
}

startSlogd(){
    checkSlogdIsStarted
    checkFileStatus
    if [ $? -ne 0 ];then
        echo "[ERROR] Failed to start slogd."
        exit 1
    fi
    ./slogd
    if [ $? -ne 0 ];then
        echo "[ERROR] Failed to start slogd."
        exit 1
    fi
    isStarted=$(ps | grep "./slogd$" | grep -v grep | wc -l)
    if [ $isStarted -ne 1 ];then
        if [ -r ""$logAgentFileDir"/slogd/slogdlog" ];then
            echo "[ERROR] Failed to start slogd. Please check "$logAgentFileDir"/slogd/slogdlog for detail."
        else
            echo "[ERROR] Failed to start slogd."
        fi
        exit 1
    fi
    echo "[INFO] Slogd started successfully."
}

restartSlogd(){
    pkill  "^slogd"
    #sleep 2000ms
    usleep 2000000
    isStopped=$(ps | grep "./slogd$" | grep -v grep | wc -l)
    if [ $isStopped -eq 1 ];then
        echo "[ERROR] Failed to kill slogd. Possibly permission denined."
        exit 1
    fi
    checkFileStatus
    if [ $? -ne 0 ];then
        echo "[ERROR] Failed to start slogd."
        exit 1
    fi
    ./slogd
    if [ $? -ne 0 ];then
        echo "[ERROR] Failed to start slogd."
        exit 1
    fi
    isStarted=$(ps | grep "./slogd$" | grep -v grep | wc -l)
    if [ $isStarted -ne 1 ];then
        if [ -r ""$logAgentFileDir"/slogd/slogdlog" ];then
            echo "[ERROR] Failed to start slogd. Please check "$logAgentFileDir"/slogd/slogdlog for detail."
        else
            echo "[ERROR] Failed to start slogd."
        fi
        exit 1
    fi
    echo "[INFO] Slogd restarted successfully."
}

stopSlogd(){
    pkill  "^slogd"
    #sleep 2000ms
    usleep 2000000
    isStopped=$(ps | grep "./slogd$" | grep -v grep | wc -l)
    if [ $isStopped -eq 1 ];then
        echo "[ERROR] Failed to kill slogd. Possibly permission denined."
        exit 1
    fi
    echo "[INFO] Slogd stopped successfully."
}

updateLogPathAndWorkPath(){
    if [ -n "$1" ]
    then
        echo "[INFO] Starting to process the log path updating request."
        str=$1
        if [ ${#str} -gt $CFG_MAX_PATH_LENGTH ];then
            echo "[ERROR] The input log path length is (${#str}), which exceeds $CFG_MAX_PATH_LENGTH."
            exit 1
        fi

        if [ ! -x "$1" ] || [ ! -r "$1" ] || [ ! -w "$1" ];then
            echo "[ERROR] The new log path=$1 does not exist or permission denied."
            exit 1
        fi
        newWorkpath=$1
        echo "[INFO] The input new path is "$newWorkpath
        if [ ! -w $configPath ]
        then
            echo "[ERROR] $configPath does not exist or permission denied."
            exit 1
        fi
        grep $LOG_AGENGT_PATH_KEY $configPath>> /dev/null 2>&1
        if [ $? -ne 0 ];then
            echo "[ERROR] Failed to find \"$LOG_AGENGT_PATH_KEY\" in "$configPath.
            exit 1
        fi
        sed -i "s?^logAgentFileDir=.*?logAgentFileDir=${newWorkpath}?g" $configPath
        if [ $? -ne 0 ];then
            echo "[ERROR] Failed to update logAgentFileDir in "$configPath.
            exit 1
        fi
        echo "[INFO] logAgentFileDir in "$configPath" has been successfully updated."

        grep $LOG_WORKSPACE_PATH_KEY $configPath>> /dev/null 2>&1
        if [ $? -ne 0 ];then
            echo "[ERROR] Failed to find \"$LOG_WORKSPACE_PATH_KEY\" in "$configPath.
            exit 1
        fi
        sed -i "s?^logWorkspace=.*?logWorkspace=${newWorkpath}?g" $configPath
        if [ $? -ne 0 ];then
            echo "[ERROR] Failed to update logWorkspace in "$configPath.
            exit 1
        fi
        echo "[INFO] logWorkspace in "$configPath" has been successfully updated."
    fi
}
checkSlogdIsStarted(){
    isStarted=$(ps | grep "./slogd$" | grep -v grep | wc -l)
    if [ $isStarted -ge 1 ];then
        echo "[INFO] Slogd is already running."
        exit 1
    fi
}
printWarnForWorkPath(){
    if [ -n "$1" ];then
        echo "[WARNING] Failed to change the log path during the restart or stop process."
    fi
}

case "$1" in
    start)
        checkSlogdIsStarted
        checkConfDir
        updateLogPathAndWorkPath $2
        startSlogd
        ;;
    restart)
        printWarnForWorkPath $2
	checkConfDir
        restartSlogd
        ;;
    stop)
        printWarnForWorkPath $2
        stopSlogd
        ;;
    *)
        echo "Please specify an action."
        echo "Usage:"
        echo "    sh set_up_slogd.sh action"
        echo "    action can be: start | restart | stop"
        echo "    start (logpath)     : start slogd"
        echo "    restart             : restart slogd"
        echo "    stop                : kill slogd if exist"
        ;;
esac

exit 0
