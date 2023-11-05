#include <algorithm>
#include <cmath>
#include <numeric>
#include <vector>

// Spearman秩相关系数
inline double calculateSpearmanRankCorrelation(const std::vector<double> &x,
                                               const std::vector<double> &y) {
  int n = x.size();
  double meanSignal = std::accumulate(x.begin(), x.end(), 0.0) / n;
  double meanReturn = std::accumulate(y.begin(), y.end(), 0.0) / n;

  double numerator = 0.0;
  double varSignal = 0.0;
  double varReturn = 0.0;

  for (int i = 0; i < n; ++i) {
    numerator += (x[i] - meanSignal) * (y[i] - meanReturn);
    varSignal += std::pow(x[i] - meanSignal, 2);
    varReturn += std::pow(y[i] - meanReturn, 2);
  }

  double ic = numerator / (std::sqrt(varSignal) * std::sqrt(varReturn));

  return ic;
}

// Sharpe Ratio
inline double calculateSharpeRatio(const std::vector<double> &returns,
                                   double riskFreeRate) {
  int n = returns.size();
  double meanReturn = std::accumulate(returns.begin(), returns.end(), 0.0) / n;
  double stdDev = 0.0;
  for (double ret : returns) {
    double diff = ret - meanReturn;
    stdDev += diff * diff;
  }
  stdDev = std::sqrt(stdDev / n) * std::sqrt(252.0);
  meanReturn *= 252;
  double excessReturn = meanReturn - riskFreeRate;
  double sharpeRatio = excessReturn / stdDev;
  return sharpeRatio;
}