#include "integral.hh"


int main(int argc, char* argv[]) {

    double res = integral::integral_with_local_stack(-10, 10, 0.00001);

    std::cout <<  res * res << std::endl;

    return 0;
}