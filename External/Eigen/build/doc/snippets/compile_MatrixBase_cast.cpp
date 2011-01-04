#include <Eigen/Core>
#include <Eigen/Array>
#include <Eigen/LU>
#include <Eigen/Cholesky>
#include <Eigen/Geometry>

using namespace Eigen;
using namespace std;

int main(int, char**)
{
  Matrix2d md = Matrix2d::Identity() * 0.45;
Matrix2f mf = Matrix2f::Identity();
cout << md + mf.cast<double>() << endl;

  return 0;
}
