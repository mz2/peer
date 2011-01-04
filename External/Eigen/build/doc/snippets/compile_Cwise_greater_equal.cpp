#include <Eigen/Core>
#include <Eigen/Array>
#include <Eigen/LU>
#include <Eigen/Cholesky>
#include <Eigen/Geometry>

using namespace Eigen;
using namespace std;

int main(int, char**)
{
  Vector3d v(1,2,3), w(3,2,1);
cout << (v.cwise()>=w) << endl;

  return 0;
}
