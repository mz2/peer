#include <Eigen/Core>
#include <Eigen/Array>
#include <Eigen/LU>
#include <Eigen/Cholesky>
#include <Eigen/Geometry>

using namespace Eigen;
using namespace std;

int main(int, char**)
{
  Vector3d v(3,2,4);
Vector3d w(5,4,2);
v.cwise() /= w;
cout << v << endl;

  return 0;
}
