#include <Eigen/Core>
#include <Eigen/Array>
#include <Eigen/LU>
#include <Eigen/Cholesky>
#include <Eigen/Geometry>

using namespace Eigen;
using namespace std;

int main(int, char**)
{
  Matrix3d m = Matrix3d::Random();
cout << "Here is the matrix m:" << endl << m << endl;
cout << "Here is the sum of each row:" << endl << m.rowwise().sum() << endl;
cout << "Here is the maximum absolute value of each row:"
     << endl << m.cwise().abs().rowwise().maxCoeff() << endl;

  return 0;
}
