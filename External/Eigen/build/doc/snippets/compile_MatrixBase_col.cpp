#include <Eigen/Core>
#include <Eigen/Array>
#include <Eigen/LU>
#include <Eigen/Cholesky>
#include <Eigen/Geometry>

using namespace Eigen;
using namespace std;

int main(int, char**)
{
  Matrix3d m = Matrix3d::Identity();
m.col(1) = Vector3d(4,5,6);
cout << m << endl;

  return 0;
}
