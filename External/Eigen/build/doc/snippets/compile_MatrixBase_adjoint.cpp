#include <Eigen/Core>
#include <Eigen/Array>
#include <Eigen/LU>
#include <Eigen/Cholesky>
#include <Eigen/Geometry>

using namespace Eigen;
using namespace std;

int main(int, char**)
{
  Matrix2cf m = Matrix2cf::Random();
cout << "Here is the 2x2 complex matrix m:" << endl << m << endl;
cout << "Here is the adjoint of m:" << endl << m.adjoint() << endl;

  return 0;
}
