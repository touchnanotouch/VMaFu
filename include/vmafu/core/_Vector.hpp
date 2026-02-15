// core/_Vector.hpp


#pragma once


#include <algorithm>
#include <cstddef>
#include <initializer_list>
#include <type_traits>


namespace vmafu {
    namespace core {
        template <typename T>
        class Vector {
            private:
                T* _data = nullptr;
                size_t _size = 0;

                // Helper methods ( memory managment )

                void _allocate(size_t n);
                void _deallocate() noexcept;

            public:
                // Constructors / Destructor

                Vector();
                explicit Vector(size_t size);
                Vector(size_t size, const T& init_value);
                Vector(std::initializer_list<T> init_list);

                ~Vector();

                // Getters

                T* data() noexcept;
                const T* data() const noexcept;

                size_t size() const noexcept;

                // Setters ( memory managment )

                void swap(Vector& other_vector) noexcept;

                void resize(size_t new_size);
                void resize(size_t new_size, T fill_value = T());

                // Copy / Move operators

                Vector(const Vector& other_vector);
                Vector(Vector&& other_vector) noexcept;

                Vector& operator=(const Vector& other_vector);
                Vector& operator=(Vector&& other_vector) noexcept;

                // Access methods

                T& operator[](size_t index) noexcept;
                const T& operator[](size_t index) const noexcept;

                T& at(size_t index);
                const T& at(size_t index) const;

                T& front() noexcept;
                const T& front() const noexcept;

                T& back() noexcept;
                const T& back() const noexcept;

                // Iterators

                T* begin() noexcept;
                const T* begin() const noexcept;

                T* end() noexcept;
                const T* end() const noexcept;

                // Static methods

                static Vector unit(size_t size, size_t index);
                static Vector zeros(size_t size);
                static Vector ones(size_t size);
                static Vector constant(size_t size, const T& fill_avalue);
                static Vector from_function(size_t size, T (*func)(T));
        };
    }
}


#include "detail/_Vector.ipp"
