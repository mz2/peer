#include <Eigen/Core>
#include <Eigen/Array>
#include <Eigen/LU>
#include <Eigen/Cholesky>
#include <Eigen/Geometry>

using namespace Eigen;
using namespace std;

int main(int, char**)
{
  Matrix3d m = Matrix3d::Zero();
m.part<Eigen::StrictlyUpperTriangular>().setOnes();
cout << "Here is the matrix m:" << endl << m << endl;
cout << "And let us now compute m*m.adjoint() in a very optimized way" << endl
     << "taking advantage of the symmetry." << endl;
Matrix3d n;
n.part<Eigen::SelfAdjoint>() = (m*m.adjoint()).lazy();
cout << "The result is:" << endl << n << endl;

  return 0;
}
