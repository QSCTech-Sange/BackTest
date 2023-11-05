#include "backtester.hxx"
#include "data_handler.hxx"
#include "date.hxx"
#include "logger.hxx"
#include "statistics.hxx"
#include <cmath>
#include <optional>
#include <ranges>

// 完整回测
void BackTester::simulate() {

  // 获得初始状态
  auto old_target_ratio = read_buckets(
      std::string(strategyPath_ / (dates_[0].toString() + ".csv")));
  if (!old_target_ratio.has_value()) {
    LOG(critical, "Missing stratregy for date {}", dates_[0].toString());
    return;
  }

  BT::Target old_target = getTargetAmount(old_target_ratio.value());

  auto old_price =
      closePriceFunc_(std::string(dataPath_ / (dates_[0].toString() + ".csv")));
  if (!old_price.has_value()) {
    LOG(critical, "Missing price for date {}", dates_[0].toString());
  }

  BT::Target old_holds = calcHolds(old_target, old_price.value());

  // 按天回测
  for (int i = 1; i < days_; ++i) {
    // 获取新状态
    auto new_price = closePriceFunc_(
        std::string(dataPath_ / (dates_[i].toString() + ".csv")));
    auto new_target_ratio = read_buckets(
        std::string(strategyPath_ / (dates_[i].toString() + ".csv")));
    auto change_price = changePriceFunc_(
        std::string(dataPath_ / (dates_[i].toString() + ".csv")));

    // 如果数据存在（当天是交易日）
    if (new_price.has_value() && new_target_ratio.has_value() &&
        change_price.has_value()) {
      LOG(info, "Calculating {}", dates_[i].toString());
      valid_dates.push_back(dates_[i].toString());
      // 计算IC
      double IC = calcIC(old_price.value(), new_price.value(),
                         new_target_ratio.value());
      IC_.push_back(IC);

      // 计算收益
      BT::Target new_target = getTargetAmount(new_target_ratio.value());
      BT::Target holds = calcHolds(new_target, new_price.value());
      double ret = calcDailyRet(old_price.value(), new_price.value(),
                                change_price.value(), old_holds, holds);
      LOG(info, "total return {}", ret);
      return_.push_back(ret / initialCash_);

      // 更新旧状态为现在状态
      old_holds = holds;
      old_price = new_price;
      old_target = new_target;
    }
  }
  sharpe_ = calculateSharpeRatio(return_, riskFreeRate_);
  saveReturnAsCSV();
}

// 从 Amount ratio 和价格得到持仓股数
BT::Target BackTester::calcHolds(const BT::Target &target,
                                 const BT::Price &price) {
  BT::Target holds;
  for (const auto &[symbol, amount] : target) {
    auto it = price.find(symbol);
    double zPrice = it == price.end() ? 0 : it->second;
    if (zPrice) {
      holds[symbol] = amount / zPrice;
    } else {
      holds[symbol] = 0;
    }
  }
  return holds;
}

// 计算每日收益，拆解为 position pnl 和 excution pnl
double BackTester::calcDailyRet(const BT::Price &old_price,
                                const BT::Price &new_price,
                                const BT::Price &change_price,
                                const BT::Target &old_holds,
                                const BT::Target &new_holds) {
  BT::Target diff = calcTargetDiff(old_holds, new_holds);
  double positionPNL =
      calcAsset(old_holds, new_price) - calcAsset(old_holds, old_price);
  LOG(info, "positionPNL {}", positionPNL);
  double excutionPNL = calcExcution(diff, change_price, new_price);
  return positionPNL + excutionPNL;
}

// 计算两个 target 之间的差异，分正负
BT::Target BackTester::calcTargetDiff(const BT::Target &old_t,
                                      const BT::Target &new_t) {
  BT::Target diff;
  for (const auto &[symbol, ratio] : old_t) {
    auto it = new_t.find(symbol);
    double new_t_ratio = it == new_t.end() ? 0 : it->second;
    diff[symbol] = new_t_ratio - ratio;
  }
  for (const auto &[symbol, ratio] : new_t) {
    if (!diff.count(symbol)) {
      diff[symbol] = ratio;
    }
  }
  return diff;
}

// 计算总资产
double BackTester::calcAsset(const BT::Target &holds, const BT::Price &price) {
  double asset = 0;
  for (const auto &[symbol, shares] : holds) {
    auto it = price.find(symbol);
    double zPrice = it == price.end() ? 0 : it->second;
    asset += zPrice * shares;
  }
  return asset;
}

// 计算执行盈亏，包含费率在内
double BackTester::calcExcution(const BT::Target &diff,
                                const BT::Price &changePrice,
                                const BT::Price &closePrice) {
  double pnl = 0;
  double tradeAmount = 0;
  for (const auto &[symbol, ratio] : diff) {
    double delta = diff.at(symbol);
    auto it = changePrice.find(symbol);
    double zPrice = it == changePrice.end() ? 0 : it->second;
    it = closePrice.find(symbol);
    double clPrice = it == closePrice.end() ? 0 : it->second;
    pnl += delta * (clPrice - zPrice);
    tradeAmount += std::abs(delta) * zPrice;
  }
  double fee = tradeAmount * excutionCost_;
  LOG(info, "excution PNL {}", pnl);
  LOG(info, "fee {}", fee);
  return pnl - fee;
}

// 从旧的 ratio target 中，归一化，并乘以 initial cash
BT::Target BackTester::getTargetAmount(const BT::Target &target) {
  double sum = std::accumulate(target.begin(), target.end(), 0.0,
                               [](double currentSum, const auto &pair) {
                                 return currentSum + pair.second;
                               });
  BT::Target newTarget;
  std::transform(
      target.begin(), target.end(), std::inserter(newTarget, newTarget.end()),
      [this, sum](const auto &pair) {
        return std::make_pair(pair.first, static_cast<double>(pair.second) /
                                              sum * initialCash_);
      });
  return newTarget;
}

// 保存回测结构
void BackTester::saveReturnAsCSV() {
  std::string timeString = BT::Date::nowStr();
  BT::Path folderPath = outputPath_ / strategyPath_.stem();

  // 检查文件夹是否存在，如果不存在，则创建
  if (!std::filesystem::exists(folderPath)) {
    std::filesystem::create_directories(folderPath);
  }

  // 保存收益曲线
  BT::Path outputPath = folderPath / (timeString + "_return.csv");
  std::ofstream outputFile(outputPath);
  LOG(info, "Going to save file {}", outputPath.string());
  auto writer = csv::make_csv_writer(outputFile);

  int n = return_.size();
  double asset = initialCash_;

  for (int i = 0; i < n; ++i) {
    writer << std::vector<std::string>({valid_dates[i], std::to_string(asset)});
    asset *= (1 + return_[i]);
  }
  outputFile.close();

  // 保存 IC
  outputPath = folderPath / (timeString + "_IC.csv");
  std::ofstream outputFileIC(outputPath);
  LOG(info, "Going to save file {}", outputPath.string());
  auto writerIC = csv::make_csv_writer(outputFileIC);

  for (int i = 0; i < n; ++i) {
    writerIC << std::vector<std::string>(
        {valid_dates[i], std::to_string(IC_[i])});
  }
  outputFileIC.close();

  // 保存 Sharpe
  outputPath = folderPath / (timeString + "_sharpe.csv");
  std::ofstream outputFileSharpe(outputPath);
  LOG(info, "Going to save file {}", outputPath.string());
  auto writerSharpe = csv::make_csv_writer(outputFileSharpe);
  writerSharpe << std::vector<std::string>({std::to_string(sharpe_)});
  outputFileSharpe.close();
}

// 计算 IC
double BackTester::calcIC(const BT::Price &old_price,
                          const BT::Price &new_price,
                          const BT::Target &target) {
  std::vector<double> real_ret;
  std::vector<double> prediction;
  for (const auto &[symbol, ratio] : target) {
    if (old_price.count(symbol) && new_price.count(symbol)) {
      prediction.push_back(ratio);
      real_ret.push_back(new_price.at(symbol) / old_price.at(symbol) - 1);
    }
  }
  for (const auto &[symbol, price] : new_price) {
    if (old_price.count(symbol) && !target.count(symbol)) {
      prediction.push_back(-1);
      real_ret.push_back(price / old_price.at(symbol) - 1);
    }
  }
  return calculateSpearmanRankCorrelation(real_ret, prediction);
}
