#include <Eigen/Core>
#include <Eigen/Array>
#include <Eigen/LU>
#include <Eigen/Cholesky>
#include <Eigen/Geometry>

using namespace Eigen;
using namespace std;

int main(int, char**)
{
  Vector3d v(1,2,3);
Vector3d w(2,3,0);
v.cwise() *= w;
cout << v << endl;

  return 0;
}
