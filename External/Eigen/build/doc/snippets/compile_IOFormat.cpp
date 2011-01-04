#include <Eigen/Core>
#include <Eigen/Array>
#include <Eigen/LU>
#include <Eigen/Cholesky>
#include <Eigen/Geometry>

using namespace Eigen;
using namespace std;

int main(int, char**)
{
  std::string sep = "\n----------------------------------------\n";
Matrix3f m1;
m1 << 1.111111, 2, 3.33333, 4, 5, 6, 7, 8.888888, 9;

IOFormat CommaInitFmt(4, Raw, ", ", ", ", "", "", " << ", ";");
IOFormat CleanFmt(4, AlignCols, ", ", "\n", "[", "]");
IOFormat OctaveFmt(4, AlignCols, ", ", ";\n", "", "", "[", "]");
IOFormat HeavyFmt(4, AlignCols, ", ", ";\n", "[", "]", "[", "]");

std::cout << m1 << sep;
std::cout << m1.format(CommaInitFmt) << sep;
std::cout << m1.format(CleanFmt) << sep;
std::cout << m1.format(OctaveFmt) << sep;
std::cout << m1.format(HeavyFmt) << sep;

  return 0;
}
