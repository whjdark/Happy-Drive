/*
 * @Author: whj
 * @Date: 2021-03-06 17:30:04
 * @LastEditors: whj
 * @LastEditTime: 2021-03-08 13:05:09
 * @Description: file content
 */

#ifndef TIMER_H
#define TIMER_H

#include <QDebug>
#include <chrono>
#include <iostream>

class Timer
{
private:
  std::chrono::time_point<std::chrono::high_resolution_clock> TimeStartPoint;
  const char* funcname;

public:
  explicit Timer(const char* _funcname)
    : TimeStartPoint(std::chrono::high_resolution_clock::now())
    , funcname(_funcname)
  {}

  ~Timer()
  {
    auto TimeEndPoint = std::chrono::high_resolution_clock::now();

    auto start =
      std::chrono::time_point_cast<std::chrono::microseconds>(TimeStartPoint)
        .time_since_epoch()
        .count();
    auto end =
      std::chrono::time_point_cast<std::chrono::microseconds>(TimeEndPoint)
        .time_since_epoch()
        .count();

    auto duration = end - start;
    double ms = 0.001 * duration;

    qDebug() << "[" << funcname << "] costs " << duration << "us(" << ms
             << "ms)";
  }
};

#endif // TIMER_H
