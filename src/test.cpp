//#include <Eigen/Dense>
//# define M_PI 3.14159265358979323846  /* pi */
//#include <iostream>
//using namespace std;
//int pasc(int k, int n);
//int factorial(int n);
//using namespace Eigen;
//
//IOFormat HeavyFmt(FullPrecision, 0, ", ", ";\n", "[", "]", "[", "]");
//
//int main() {
//    // parameters
//    int queue_size_ = 25;
//    int window_size_ = 4;
//    int kernel_size_ = 5;
//    double harris_threshold_ = 8.0;
//    Eigen::MatrixXd Gx_, h_;
//
//    Eigen::VectorXd Dx = Eigen::VectorXd(kernel_size_);
//    Eigen::VectorXd Sx = Eigen::VectorXd(kernel_size_);
//    for (int i = 0; i < kernel_size_; i++)
//    {
//        Sx[i] = factorial(kernel_size_ - 1) /
//            (factorial(kernel_size_ - 1 - i) * factorial(i));
//        Dx[i] = pasc(i, kernel_size_ - 2) - pasc(i - 1, kernel_size_ - 2);
//    }
//    Gx_ = Sx * Dx.transpose();
//    Gx_ = Gx_ / Gx_.maxCoeff();
//
//    const double sigma = 1.;
//    const double A = 1. / (2. * M_PI * sigma * sigma);
//    const int l2 = (2 * window_size_ + 2 - kernel_size_) / 2;
//    h_ = Eigen::MatrixXd(2 * l2 + 1, 2 * l2 + 1);
//    for (int x = -l2; x <= l2; x++)
//    {
//        for (int y = -l2; y <= l2; y++)
//        {
//            const double h_xy = A * exp(-(x * x + y * y) / (2 * sigma * sigma));
//            h_(l2 + x, l2 + y) = h_xy;
//        }
//    }
//    h_ /= h_.sum();
//    std::string sep = "\n----------------------------------------\n";
//
//    std::cout << Gx_.format(HeavyFmt) << sep;
//
//    int k = 0;
//
//}
//
//
//int factorial(int n) {
//    if (n > 1)
//    {
//        return n * factorial(n - 1);
//    }
//    else
//    {
//        return 1;
//    }
//}
//
//int pasc(int k, int n) {
//    if (k >= 0 && k <= n)
//    {
//        return factorial(n) / (factorial(n - k) * factorial(k));
//    }
//    else
//    {
//        return 0;
//    }
//
//}