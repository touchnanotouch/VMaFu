// core/_Function.hpp


#pragma once


#include <functional>
#include <type_traits>
#include <cstddef>


namespace vmafu {
    // Forward declaration

    template <typename ResultT, typename... ArgTs>
    class Function;
    
    // Function 1-d class

    template <typename ResultT, typename ArgT>
    class Function<ResultT, ArgT> {
        private:
            std::function<ResultT(ArgT)> _func;

        public:
            // Constructors / Destructor

            Function();
            explicit Function(std::function<ResultT(ArgT)> func);

            ~Function();

            // Getter

            bool is_initialized() const noexcept;

            // Copy / Move operators

            Function(const Function&);
            Function(Function&&) noexcept;

            Function& operator=(const Function&);
            Function& operator=(Function&&) noexcept;

            // Access methods

            ResultT operator()(ArgT x) const;

            ResultT at(ArgT x) const;

            // Static methods

            static Function constant(ResultT value);

            static Function from_function_ptr(ResultT (*func)(ArgT));
    };

    // Function 2-d class

    template <typename ResultT, typename ArgT1, typename ArgT2>
    class Function<ResultT, ArgT1, ArgT2> {
        private:
            std::function<ResultT(ArgT1, ArgT2)> _func;

        public:
            // Constructors / Destructor

            Function();
            explicit Function(std::function<ResultT(ArgT1, ArgT2)> func);

            ~Function();

            // Getter

            bool is_initialized() const noexcept;

            // Copy / Move operators

            Function(const Function&);
            Function(Function&&) noexcept;

            Function& operator=(const Function&);
            Function& operator=(Function&&) noexcept;

            // Access methods

            ResultT operator()(ArgT1 x, ArgT2 y) const;

            ResultT at(ArgT1 x, ArgT2 y) const;

            // Static methods

            static Function constant(ResultT value);

            static Function from_function_ptr(ResultT (*func)(ArgT1, ArgT2));
        };

    // Function 3-d class

    template <typename ResultT, typename ArgT1, typename ArgT2, typename ArgT3>
    class Function<ResultT, ArgT1, ArgT2, ArgT3> {
        private:
            std::function<ResultT(ArgT1, ArgT2, ArgT3)> _func;

        public:
            // Constructors / Destructor

            Function();
            explicit Function(std::function<ResultT(ArgT1, ArgT2, ArgT3)> func);

            ~Function();

            // Getter

            bool is_initialized() const noexcept;

            // Copy / Move operators

            Function(const Function&);
            Function(Function&&) noexcept;

            Function& operator=(const Function&);
            Function& operator=(Function&&) noexcept;

            // Access methods

            ResultT operator()(ArgT1 x, ArgT2 y, ArgT3 z) const;

            ResultT at(ArgT1 x, ArgT2 y, ArgT3 z) const;

            // Static methods

            static Function constant(ResultT value);

            static Function from_function_ptr(
                ResultT (*func)(ArgT1, ArgT2, ArgT3)
            );
    };

    // Aliases for simplicity

    template <typename T>
    using Function1D = Function<T, T>;
    
    template <typename T>
    using Function2D = Function<T, T, T>;
    
    template <typename T>  
    using Function3D = Function<T, T, T, T>;
}


#include "detail/_Function.ipp"
