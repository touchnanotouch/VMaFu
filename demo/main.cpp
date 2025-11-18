#include <iostream>

// #include <vmafu/core/Vector.hpp>
#include <vmafu/core/Matrix.hpp>


int main() {
    // vmafu::Vector<double> v(5);
    // for (size_t i = 0; i < v.size(); i++) {
    //     v[i] = i * 1.5;
    // }
    
    // std::cout << v << std::endl;
    // std::cout << "Norm: " << v.norm() << std::endl;

    // std::cout << vmafu::Vector<double>(0) << std::endl;
    // std::cout << vmafu::Vector<int>::unit_vector(5, 1) << std::endl;
    std::cout << vmafu::Matrix<int>::ones(4, 4) << std::endl;
    
    return 0;
}
