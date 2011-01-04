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
cout << "Here is the matrix m:" << endl << m << endl;
cout << "Here is the bottom-right 2x3 corner in m:" << endl
     << m.corner(Eigen::BottomRight, 2, 3) << endl;
m.corner(Eigen::BottomRight, 2, 3).setZero();
cout << "Now the matrix m is:" << endl << m << endl;

  return 0;
}
