#include <Eigen/Core>
#include <Eigen/Array>
#include <Eigen/LU>
#include <Eigen/Cholesky>
#include <Eigen/Geometry>

using namespace Eigen;
using namespace std;

int main(int, char**)
{
  Vector3d v(2,3,4), w(4,2,3);
cout << v.cwise().max(w) << endl;

  return 0;
}
