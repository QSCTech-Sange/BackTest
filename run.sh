#!/bin/bash

# 设置环境变量
export BACKTEST_PATH="/home/jimmy/projects/WenBo/Question2"
export BACKTEST_LOG="${BACKTEST_PATH}/logs"
export BACKTEST_DATA="${BACKTEST_PATH}/data"
export BACKTEST_STRATEGY="${BACKTEST_PATH}/strategy/TEST001"
export BACKTEST_DATA_SOURCE="DongCai"
export BACKTEST_START="2022-11-04"
export BACKTEST_END="2023-11-02"
export BACKTEST_EXECUTION_COST="0.00045"
export BACKTEST_INITAIL_CASH="10000000"
export BACKTEST_OUTPUT="${BACKTEST_PATH}/result/"
export BACKTEST_RISK_FREE_RATE="0.015"

# 运行BackTest程序
./build/BackTest