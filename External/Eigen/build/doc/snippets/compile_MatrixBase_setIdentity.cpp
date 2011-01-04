#include <Eigen/Core>
#include <Eigen/Array>
#include <Eigen/LU>
#include <Eigen/Cholesky>
#include <Eigen/Geometry>

using namespace Eigen;
using namespace std;

int main(int, char**)
{
  Matrix4i m = Matrix4i::Zero();
m.block<3,3>(1,0).setIdentity();
cout << m << endl;

  return 0;
}
