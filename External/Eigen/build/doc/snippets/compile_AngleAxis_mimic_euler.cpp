#include <Eigen/Core>
#include <Eigen/Array>
#include <Eigen/LU>
#include <Eigen/Cholesky>
#include <Eigen/Geometry>

using namespace Eigen;
using namespace std;

int main(int, char**)
{
  Matrix3f m;
m = AngleAxisf(0.25*M_PI, Vector3f::UnitX())
  * AngleAxisf(0.5*M_PI,  Vector3f::UnitY())
  * AngleAxisf(0.33*M_PI, Vector3f::UnitZ());
cout << m << endl << "is unitary: " << m.isUnitary() << endl;

  return 0;
}
