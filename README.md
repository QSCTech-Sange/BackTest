# BackTest
一个简易的回测系统，可以计算 IC、Sharpe、收益曲线。

## 编译
```
cd build
cmake ..
make
```
- 为了方便面试官审阅，这里 build 并没有按照开源项目规范直接清空，保留了可执行文件，因此可以跳过编译步骤。

## 回测
首先在 `run.sh` 里设置好对应的参数，可以参考给出的示例
```
#!/bin/bash

# 设置环境变量
# 基础路径
export BACKTEST_PATH="/home/jimmy/projects/WenBo/Question2"
# 日志路径
export BACKTEST_LOG="${BACKTEST_PATH}/logs"
# 数据路径
export BACKTEST_DATA="${BACKTEST_PATH}/data"
# 策略路径
export BACKTEST_STRATEGY="${BACKTEST_PATH}/strategy/TEST002"
# 数据源格式
export BACKTEST_DATA_SOURCE="DongCai"
# 回测开始日期
export BACKTEST_START="2022-11-04"
# 回测结束日期
export BACKTEST_END="2023-11-02"
# 交易执行成本（手续费）
export BACKTEST_EXECUTION_COST="0.00045"
# 初始资金
export BACKTEST_INITAIL_CASH="10000000"
# 保存结果
export BACKTEST_OUTPUT="${BACKTEST_PATH}/result/"
# 无风险年利率
export BACKTEST_RISK_FREE_RATE="0.015"

# 运行BackTest程序，注意路径改为程序所在路径
./build/BackTest
```
然后添加一下权限
```
chmod +x run.sh
```
之后，只需要运行
```
./run.sh
```
即可。

## 回测结果
回测结果分为三个文件，分别为
```
指定结果路径/策略名/时间_IC.csv
指定结果路径/策略名/时间_return.csv
指定结果路径/策略名/时间_sharpe.csv
``````
我也写了一个 jupyter notebook 方便展示这些数据。

## 项目结构
项目主体由 cxx、hxx 和一个 CMakeLists.txt 组成。
几个模块包含
- `backtester` 最为主要的类，包含了回测的绝大多数功能
- `statistics` 包含了一些统计上的计算函数
- `logger` 用一个单例简单封装了 spdlog
- `spdlog` 第三方的日志库
- `csv` 第三方的csv解析库
- `data_handler` 数据解析函数
- `types` 类型别名
- `date` 简单的日期
- `config_parser` 解析环境变量的函数
- `main` 解析环境变量并运行回测 

辅助性的有 build、data、strategy、result、logs、README.md 等，主要便于方便批改和审阅，其中很多数据是自动生成的，可以忽略。
## 其他说明
### 数据
- 原数据来源于 ArkShare 的东财数据。我在 data 目录下也放了一个 python 脚本用于获取股票数据。可以做相应修改以下载想要的数据。
- 如果有其他数据，可以在 `data_handler` 中编写其他的数据解析函数。

## 策略
- 假定策略是一个 buckets 的形式，格式为一行一个股票代码和一个 ratio，参加示例数据。
- 我的模拟策略生成自东财数据，TEST001 为作弊的使用未来数据的策略，TEST002基本上就是一个胡乱的策略。可以参考 `gen_strategy.ipynb`

### 优点
- 配备了策略数据和行情数据以及获取/生成它们的代码
- 以头文件的形式引入第三方库 spdlog 和 csv 解析器
- 有完善的 log
- 使用 CMake 来管理工程
- 使用 C++ 20 的 ranges，C++ 17 的 optional，filesystem，结构化绑定，string_view，NRVO 等现代C++的机制和功能。
- 使用了单例模式等设计模式

### 缺陷
- 数据校验等异常处理，做得并不严格。
- 没有来得及做并行化处理。
- 由于缺乏数据，换仓价格设定的为(今天收盘价-昨日收盘价)/2，严格来说不是很合理。
- 只能计算 IC、收益曲线以及Sharpe。