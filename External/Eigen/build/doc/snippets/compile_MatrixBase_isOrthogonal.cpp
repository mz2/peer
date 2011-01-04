#include <Eigen/Core>
#include <Eigen/Array>
#include <Eigen/LU>
#include <Eigen/Cholesky>
#include <Eigen/Geometry>

using namespace Eigen;
using namespace std;

int main(int, char**)
{
  Vector3d v(1,0,0);
Vector3d w(1e-4,0,1);
cout << "Here's the vector v:" << endl << v << endl;
cout << "Here's the vector w:" << endl << w << endl;
cout << "v.isOrthogonal(w) returns: " << v.isOrthogonal(w) << endl;
cout << "v.isOrthogonal(w,1e-3) returns: " << v.isOrthogonal(w,1e-3) << endl;

  return 0;
}
