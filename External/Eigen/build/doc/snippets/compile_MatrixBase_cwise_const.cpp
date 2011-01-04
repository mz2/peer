#include <Eigen/Core>
#include <Eigen/Array>
#include <Eigen/LU>
#include <Eigen/Cholesky>
#include <Eigen/Geometry>

using namespace Eigen;
using namespace std;

int main(int, char**)
{
  Vector3d v(-1,2,-3);
cout << "the absolute values:" << endl << v.cwise().abs() << endl;
cout << "the absolute values plus one:" << endl << v.cwise().abs().cwise()+1 << endl;
cout << "sum of the squares: " << v.cwise().square().sum() << endl;

  return 0;
}
