#include <Eigen/Core>
#include <Eigen/Array>
#include <Eigen/LU>
#include <Eigen/Cholesky>
#include <Eigen/Geometry>

using namespace Eigen;
using namespace std;

int main(int, char**)
{
  MatrixXf m = MatrixXf::Random(3,5);
cout << "Here is the matrix m:" << endl << m << endl;
MatrixXf ker = m.lu().kernel();
cout << "Here is a matrix whose columns form a basis of the kernel of m:"
     << endl << ker << endl;
cout << "By definition of the kernel, m*ker is zero:"
     << endl << m*ker << endl;

  return 0;
}
