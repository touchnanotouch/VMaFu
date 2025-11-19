#pragma once


#include <algorithm>
#include <cmath>
#include <ostream>
#include <stdexcept>
#include <initializer_list>


namespace vmafu {
    template <typename T>
    class Vector {
        private:
            T* _data = nullptr;
            size_t _size = 0;

            double _eps = 1e-10;

            // Checks

            void _check_index(size_t index) const;
            void _check_size_match(const Vector& rhs, const char* op) const;
            void _check_divisor(T scalar) const;
            void _check_non_empty(const char* op) const;

            bool _check_size_equal(const Vector& rhs) const noexcept;
            bool _check_size_almost_equal(const Vector& rhs) const noexcept;

        public:
            // Constructors/Destructor

            Vector();
            Vector(size_t size);
            Vector(size_t size, T init_val);
            Vector(std::initializer_list<T> init);

            ~Vector();

            // Getters

            size_t size() const noexcept;

            double eps() const noexcept;

            bool empty() const noexcept;

            // Setters

            void set_eps(double new_eps);
            void resize(size_t new_size, T value = T());
            void swap(Vector& other) noexcept;

            // Copy/Move operators

            Vector(const Vector& other);
            Vector(Vector&& other) noexcept;
            Vector& operator=(const Vector& other);
            Vector& operator=(Vector&& other) noexcept;

            // Access operators

            T& operator[](size_t index);
            const T& operator[](size_t index) const;

            // Iterators

            T* begin() noexcept;
            T* end() noexcept;
            const T* begin() const noexcept;
            const T* end() const noexcept;

            // Arithmetic operators

            Vector operator+(const Vector& rhs) const;
            Vector operator-(const Vector& rhs) const;
            Vector operator*(T scalar) const;
            Vector operator/(T scalar) const;

            Vector& operator+=(const Vector& rhs);
            Vector& operator-=(const Vector& rhs);
            Vector& operator*=(T scalar);
            Vector& operator/=(T scalar);

            // Linear algebra operators
            
            T sum() const;
            T dot(const Vector& rhs) const;

            double norm() const;
            T norm_max() const;

            Vector normalized() const;
            Vector project_onto(const Vector& basis) const;

            bool is_orthogonal_to(const Vector& other) const;
            bool is_collinear_with(const Vector& other) const;

            Vector element_wise_multiply(const Vector& other) const;
            Vector linear_combination(const Vector& other, T alpha, T beta) const;

            // Comparison operators

            bool operator==(const Vector& rhs) const;
            bool operator!=(const Vector& rhs) const;

            // Static methods
    
            static Vector unit_vector(size_t size, size_t index);
            static Vector from_function(size_t size, T (*func)(size_t));

            // Friend methods

            template <typename U>
            friend std::ostream& operator<<(std::ostream& os, const Vector<U>& v);
    };

    // External methods

    template <typename T>
    Vector<T> operator*(T scalar, const Vector<T>& vec);
}


#include "detail/Vector.ipp"
