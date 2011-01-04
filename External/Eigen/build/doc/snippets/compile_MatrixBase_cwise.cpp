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
v.cwise() += 3;
v.cwise() -= 2;
cout << v << endl;

  return 0;
}
