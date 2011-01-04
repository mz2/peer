#include <Eigen/Core>
#include <Eigen/Array>
#include <Eigen/LU>
#include <Eigen/Cholesky>
#include <Eigen/Geometry>

using namespace Eigen;
using namespace std;

int main(int, char**)
{
  Matrix4i m = Matrix4i::Random();
m.row(1).setZero();
cout << m << endl;

  return 0;
}
