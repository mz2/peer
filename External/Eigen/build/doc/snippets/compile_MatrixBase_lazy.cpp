#include <Eigen/Core>
#include <Eigen/Array>
#include <Eigen/LU>
#include <Eigen/Cholesky>
#include <Eigen/Geometry>

using namespace Eigen;
using namespace std;

int main(int, char**)
{
  Matrix2d m; m << 1,2,3,4;
cout << (m*m).lazy().row(0) << endl;
 // this computes only one row of the product. By contrast,
 // if we did "(m*m).row(0);" then m*m would first be evaluated into
 // a temporary, because the Product expression has the EvalBeforeNestingBit.

  return 0;
}
