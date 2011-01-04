#include <Eigen/Core>
#include <Eigen/Array>
#include <Eigen/LU>
#include <Eigen/Cholesky>
#include <Eigen/Geometry>

using namespace Eigen;
using namespace std;

int main(int, char**)
{
  int rows=5, cols=5;
MatrixXf m(rows,cols);
m << (Matrix3f() << 1, 2, 3, 4, 5, 6, 7, 8, 9).finished(),
     MatrixXf::Zero(3,cols-3),
     MatrixXf::Zero(rows-3,3),
     MatrixXf::Identity(rows-3,cols-3);
cout << m;

  return 0;
}
