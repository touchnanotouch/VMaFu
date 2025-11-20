#include <iostream>
#include <fstream>
#include <cmath>
#include <vector>

#include <vmafu/algorithms/inteq/FredholmIntEq.hpp>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Функция для сохранения данных в CSV файл
template<typename T>
void save_to_csv(const std::string& filename, const std::vector<T>& x_values, 
                const std::vector<T>& y_values, const std::string& header = "x,y") {
    std::ofstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open file: " + filename);
    }
    
    file << header << "\n";
    for (size_t i = 0; i < x_values.size(); ++i) {
        file << x_values[i] << "," << y_values[i] << "\n";
    }
    file.close();
    
    std::cout << "Data saved to: " << filename << " (" << x_values.size() << " points)" << std::endl;
}

// Аналитическое решение для тестового уравнения (если известно)
double analytical_solution(double x) {
    return 1.5 * x;
}

int main() {
    try {
        // Параметры вычислений
        const size_t n_write = 100;  // Количество точек для записи в файл
        const int n_basis = 4;
        const int n_points = 8;
        const double a = 0.0, b = 1.0;
        const double lambda = 1.0;
        
        // Ядро и свободный член
        vmafu::Function2D<double> kernel([](double x, double t) { return x * t; });
        vmafu::Function1D<double> free_term([](double x) { return x; });
        
        // Создаем решатель
        vmafu::FredholmIntEq<double> solver(kernel, free_term, lambda, a, b);
        
        // Конфигурации для методов
        vmafu::IntegralEquationConfig<double> config_collocation(
            vmafu::IntegralEquationType::FredholmIntEq,
            vmafu::SolutionMethod::Collocation,
            lambda, a, b, n_basis, n_points
        );

        vmafu::IntegralEquationConfig<double> config_galerkin(
            vmafu::IntegralEquationType::FredholmIntEq,
            vmafu::SolutionMethod::Galerkin,
            lambda, a, b, n_basis, n_points
        );

        // Решаем уравнения
        std::cout << "Solving integral equation..." << std::endl;
        auto solution_collocation = solver.solve(config_collocation);
        auto solution_galerkin = solver.solve(config_galerkin);
        
        std::cout << "Solutions computed successfully!" << std::endl;

        // Подготавливаем данные для записи
        std::vector<double> x_values(n_write);
        std::vector<double> y_collocation(n_write);
        std::vector<double> y_galerkin(n_write);
        std::vector<double> y_analytical(n_write);
        
        // Вычисляем значения в точках
        for (size_t i = 0; i < n_write; ++i) {
            double x = a + (b - a) * static_cast<double>(i) / (n_write - 1);
            x_values[i] = x;
            y_collocation[i] = solution_collocation(x);
            y_galerkin[i] = solution_galerkin(x);
            y_analytical[i] = analytical_solution(x);
        }

        // Сохраняем результаты в CSV файлы
        // save_to_csv("../collocation_solution.csv", x_values, y_collocation, "x,collocation");
        // save_to_csv("../galerkin_solution.csv", x_values, y_galerkin, "x,galerkin");
        // save_to_csv("../analytical_solution.csv", x_values, y_analytical, "x,analytical");
        
        // Создаем объединенный файл для удобства сравнения
        std::ofstream combined_file("../data/combined_solutions.csv");
        combined_file << "x,collocation,galerkin,analytical\n";
        for (size_t i = 0; i < n_write; ++i) {
            combined_file << x_values[i] << "," 
                         << y_collocation[i] << "," 
                         << y_galerkin[i] << "," 
                         << y_analytical[i] << "\n";
        }
        combined_file.close();
        std::cout << "Combined data saved to: combined_solutions.csv" << std::endl;

        // Вычисляем невязки
        double residual_collocation = solver.calculate_residual(solution_collocation, 1000);
        double residual_galerkin = solver.calculate_residual(solution_galerkin, 1000);
        
        std::cout << "\nResiduals:" << std::endl;
        std::cout << "Collocation method: " << residual_collocation << std::endl;
        std::cout << "Galerkin method: " << residual_galerkin << std::endl;
        
        // Выводим несколько значений для быстрой проверки
        std::cout << "\nSample values at x=0.75:" << std::endl;
        std::cout << "Collocation: " << solution_collocation(0.75) << std::endl;
        std::cout << "Galerkin: " << solution_galerkin(0.75) << std::endl;
        std::cout << "Analytical: " << analytical_solution(0.75) << std::endl;

    }
    catch(const std::exception& e) {
        std::cerr << "Error: " << e.what() << '\n';
        return 1;
    }

    return 0;
}
