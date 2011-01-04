#include <Eigen/Core>
#include <Eigen/Array>
#include <Eigen/LU>
#include <Eigen/Cholesky>
#include <Eigen/Geometry>

using namespace Eigen;
using namespace std;

int main(int, char**)
{
  Matrix3i a = Matrix3i::Random(), b = Matrix3i::Random();
Matrix3i c = a.cwise() * b;
cout << "a:\n" << a << "\nb:\n" << b << "\nc:\n" << c << endl;


  return 0;
}
