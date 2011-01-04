#include <Eigen/Core>
#include <Eigen/Array>
#include <Eigen/LU>
#include <Eigen/Cholesky>
#include <Eigen/Geometry>

using namespace Eigen;
using namespace std;

int main(int, char**)
{
  RowVector4i v = RowVector4i::Random();
cout << "Here is the vector v:" << endl << v << endl;
cout << "Here is v.end(2):" << endl << v.end(2) << endl;
v.end(2).setZero();
cout << "Now the vector v is:" << endl << v << endl;

  return 0;
}
