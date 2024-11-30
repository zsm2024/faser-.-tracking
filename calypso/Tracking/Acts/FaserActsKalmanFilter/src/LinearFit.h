#ifndef FASERACTSKALMANFILTER_LINEARFIT_H
#define FASERACTSKALMANFILTER_LINEARFIT_H

#include "Acts/Definitions/Algebra.hpp"

namespace LinearFit {

using Vector2 = Acts::Vector2;

std::pair <Vector2, Vector2 > linearFit(const std::vector<Vector2> &points) {
  size_t nPoints = points.size();
  Eigen::Matrix< Vector2::Scalar, Eigen::Dynamic, Eigen::Dynamic > centers(nPoints, 2);
  for (size_t i = 0; i < nPoints; ++i) centers.row(i) = points[i];

  Vector2 origin = centers.colwise().mean();
  Eigen::MatrixXd centered = centers.rowwise() - origin.transpose();
  Eigen::MatrixXd cov = centered.adjoint() * centered;
  Eigen::SelfAdjointEigenSolver<Eigen::MatrixXd> eig(cov);
  Vector2 axis = eig.eigenvectors().col(1).normalized();

  return std::make_pair(origin, axis);
}

}  // namespace LinearFit

#endif  // FASERACTSKALMANFILTER_LINEARFIT_H
