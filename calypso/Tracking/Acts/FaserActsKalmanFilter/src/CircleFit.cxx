#include "CircleFit.h"

namespace CircleFit {

CircleData::CircleData(const std::vector<const Tracker::FaserSCT_SpacePoint *> &spacePoints) {
  for (auto sp: spacePoints) {
    m_x.push_back(sp->globalPosition().z());
    m_y.push_back(sp->globalPosition().y());
  }
  m_size = spacePoints.size();
}

CircleData::CircleData(const std::vector<Amg::Vector3D> &spacePoints) {
  for (auto sp: spacePoints) {
    m_x.push_back(sp.z());
    m_y.push_back(sp.y());
  }
  m_size = spacePoints.size();
}

double CircleData::meanX() const {
  double mean = 0;
  for (double i: m_x) mean += i;
  return mean / m_size;
}

double CircleData::meanY() const {
  double mean = 0;
  for (double i: m_y) mean += i;
  return mean / m_size;
}

double CircleData::x(int i) const {
  return m_x[i];
}

double CircleData::y(int i) const {
  return m_y[i];
}

int CircleData::size() const {
  return m_size;
}


double sigma(const CircleData &data, const Circle &circle) {
  double sum=0.,dx,dy;
  for (int i=0; i<data.size(); i++) {
    dx = data.x(i) - circle.cx;
    dy = data.y(i) - circle.cy;
    sum += (sqrt(dx*dx+dy*dy) - circle.r) * (sqrt(dx*dx+dy*dy) - circle.r);
  }
  return sqrt(sum/data.size());
}


Circle circleFit(const CircleData &data)
/*
      Circle fit to a given set of data points (in 2D)

      This is an algebraic fit, due to Taubin, based on the journal article

      G. Taubin, "Estimation Of Planar Curves, Surfaces And Nonplanar
                  Space Curves Defined By Implicit Equations, With
                  Applications To Edge And Range Image Segmentation",
                  IEEE Trans. PAMI, Vol. 13, pages 1115-1138, (1991)

      Input:  data     - the class of data (contains the given points):

	      data.size()   - the number of data points
	      data.X[] - the array of X-coordinates
	      data.Y[] - the array of Y-coordinates

     Output:
               circle - parameters of the fitting circle:

	       circle.a - the X-coordinate of the center of the fitting circle
	       circle.b - the Y-coordinate of the center of the fitting circle
 	       circle.r - the radius of the fitting circle
 	       circle.s - the root mean square error (the estimate of sigma)
 	       circle.j - the total number of iterations

     The method is based on the minimization of the function

                  sum [(x-a)^2 + (y-b)^2 - R^2]^2
              F = -------------------------------
                      sum [(x-a)^2 + (y-b)^2]

     This method is more balanced than the simple Kasa fit.

     It works well whether data points are sampled along an entire circle or
     along a small arc.

     It still has a small bias and its statistical accuracy is slightly
     lower than that of the geometric fit (minimizing geometric distances),
     but slightly higher than that of the very similar Pratt fit.
     Besides, the Taubin fit is slightly simpler than the Pratt fit

     It provides a very good initial guess for a subsequent geometric fit.

       Nikolai Chernov  (September 2012)

*/
{
  int i, iter, IterMAX = 999;

  double Xi, Yi, Zi;
  double Mz, Mxy, Mxx, Myy, Mxz, Myz, Mzz, Cov_xy, Var_z;
  double A0, A1, A2, A22, A3, A33;
  double Dy, xnew, x, ynew, y;
  double DET, Xcenter, Ycenter;

  Circle circle;

  Mxx = Myy = Mxy = Mxz = Myz = Mzz = 0.;
  for (i = 0; i < data.size(); i++) {
    Xi = data.x(i) - data.meanX();
    Yi = data.y(i) - data.meanY();
    Zi = Xi * Xi + Yi * Yi;

    Mxy += Xi * Yi;
    Mxx += Xi * Xi;
    Myy += Yi * Yi;
    Mxz += Xi * Zi;
    Myz += Yi * Zi;
    Mzz += Zi * Zi;
  }
  Mxx /= data.size();
  Myy /= data.size();
  Mxy /= data.size();
  Mxz /= data.size();
  Myz /= data.size();
  Mzz /= data.size();

  Mz = Mxx + Myy;
  Cov_xy = Mxx * Myy - Mxy * Mxy;
  Var_z = Mzz - Mz * Mz;
  A3 = 4 * Mz;
  A2 = -3 * Mz * Mz - Mzz;
  A1 = Var_z * Mz + 4 * Cov_xy * Mz - Mxz * Mxz - Myz * Myz;
  A0 = Mxz * (Mxz * Myy - Myz * Mxy) + Myz * (Myz * Mxx - Mxz * Mxy) - Var_z * Cov_xy;
  A22 = A2 + A2;
  A33 = A3 + A3 + A3;

  for (x = 0., y = A0, iter = 0; iter < IterMAX; iter++) {
    Dy = A1 + x * (A22 + A33 * x);
    xnew = x - y / Dy;
    if ((xnew == x) || (!std::isfinite(xnew))) break;
    ynew = A0 + xnew * (A1 + xnew * (A2 + xnew * A3));
    if (abs(ynew) >= abs(y)) break;
    x = xnew;
    y = ynew;
  }

  DET = x * x - x * Mz + Cov_xy;
  Xcenter = (Mxz * (Myy - x) - Myz * Mxy) / DET / 2;
  Ycenter = (Myz * (Mxx - x) - Mxz * Mxy) / DET / 2;

  circle.cx = Xcenter + data.meanX();
  circle.cy = Ycenter + data.meanY();
  circle.r = sqrt(Xcenter * Xcenter + Ycenter * Ycenter + Mz);
  circle.s = sigma(data, circle);
  circle.i = 0;
  circle.j = iter;

  return circle;
}

}
