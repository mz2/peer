#include <Eigen/Core>
#include <Eigen/Array>
#include <Eigen/LU>
#include <Eigen/Cholesky>
#include <Eigen/Geometry>

using namespace Eigen;
using namespace std;

int main(int, char**)
{
  Matrix3i m = Matrix3i::Random();
cout << "Here is the matrix m:" << endl << m << endl;
cout << "Here is m.minor(1,1):" << endl << m.minor(1,1) << endl;

  return 0;
}
