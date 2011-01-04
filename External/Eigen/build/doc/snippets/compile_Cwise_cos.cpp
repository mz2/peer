#include <Eigen/Core>
#include <Eigen/Array>
#include <Eigen/LU>
#include <Eigen/Cholesky>
#include <Eigen/Geometry>

using namespace Eigen;
using namespace std;

int main(int, char**)
{
  Vector3d v(M_PI, M_PI/2, M_PI/3);
cout << v.cwise().cos() << endl;

  return 0;
}
