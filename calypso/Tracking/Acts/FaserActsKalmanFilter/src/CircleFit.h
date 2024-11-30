#ifndef FASERACTSKALMANFILTER_CIRCLEFIT_H
#define FASERACTSKALMANFILTER_CIRCLEFIT_H

#include "TrackerSpacePoint/FaserSCT_SpacePoint.h"
#include <cmath>
#include <vector>

namespace CircleFit {


class CircleData {
public:
  CircleData(const std::vector<const Tracker::FaserSCT_SpacePoint *> &spacePoints);
  CircleData(const std::vector<Amg::Vector3D> &spacePoints);
  double meanX() const;
  double meanY() const;
  double x(int i) const;
  double y(int i) const;
  int size() const;

private:
  std::vector<double> m_x{};
  std::vector<double> m_y{};
  int m_size = 0;
};


struct Circle {
public:
  Circle() = default;
  Circle(double cx, double cy, double r) : cx(cx), cy(cy), r(r) {};

  double cx = 0;
  double cy = 0;
  double r = 0;
  double s = 0;
  double i = 0;
  double j = 0;
};

double sigma(const CircleData &data, const Circle &circle);
Circle circleFit(const CircleData &data);

}  // namespace CircleFit

#endif // FASERACTSKALMANFILTER_CIRCLEFIT_H
