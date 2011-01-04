#include <Eigen/Core>
#include <Eigen/Array>
#include <Eigen/LU>
#include <Eigen/Cholesky>
#include <Eigen/Geometry>

using namespace Eigen;
using namespace std;

int main(int, char**)
{
  cout << 6 * RowVectorXi::Ones(4) << endl;
cout << VectorXf::Ones(2) << endl;

  return 0;
}
