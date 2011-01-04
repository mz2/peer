#include <Eigen/Core>
#include <Eigen/Array>
#include <Eigen/LU>
#include <Eigen/Cholesky>
#include <Eigen/Geometry>

using namespace Eigen;
using namespace std;

int main(int, char**)
{
  Matrix4d m = Vector4d(1,2,3,4).asDiagonal();
cout << "Here is the matrix m:" << endl << m << endl;
cout << "Here is m.fixed<2, 2>(2, 2):" << endl << m.block<2, 2>(2, 2) << endl;
m.block<2, 2>(2, 0) = m.block<2, 2>(2, 2);
cout << "Now the matrix m is:" << endl << m << endl;

  return 0;
}
