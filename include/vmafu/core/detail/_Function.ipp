// core/detail/_Function.ipp


namespace vmafu {
    namespace core {
        // Macros

        #define FUNCTION_TEMPLATE_1D template <typename ResultT, typename ArgT>
        #define FUNCTION_TEMPLATE_2D template <typename ResultT, typename ArgT1, typename ArgT2>
        #define FUNCTION_TEMPLATE_3D template <typename ResultT, typename ArgT1, typename ArgT2, typename ArgT3>

        #define FUNCTION_TYPE_1D Function<ResultT, ArgT>
        #define FUNCTION_TYPE_2D Function<ResultT, ArgT1, ArgT2>  
        #define FUNCTION_TYPE_3D Function<ResultT, ArgT1, ArgT2, ArgT3>

        // Function 1-d class

        // Constructors / Destructor

        FUNCTION_TEMPLATE_1D
        FUNCTION_TYPE_1D::Function() = default;

        FUNCTION_TEMPLATE_1D
        FUNCTION_TYPE_1D::Function(
            std::function<ResultT(ArgT)> func
        ) : _func(std::move(func)) {}

        FUNCTION_TEMPLATE_1D
        FUNCTION_TYPE_1D::~Function() = default;

        // Getter

        FUNCTION_TEMPLATE_1D
        bool FUNCTION_TYPE_1D::is_initialized() const noexcept {
            return static_cast<bool>(_func);
        }

        // Copy / Move operators

        FUNCTION_TEMPLATE_1D
        FUNCTION_TYPE_1D::Function(const Function&) = default;

        FUNCTION_TEMPLATE_1D
        FUNCTION_TYPE_1D::Function(Function&&) noexcept = default;

        FUNCTION_TEMPLATE_1D
        FUNCTION_TYPE_1D& FUNCTION_TYPE_1D::operator=(const Function&) = default;

        FUNCTION_TEMPLATE_1D
        FUNCTION_TYPE_1D& FUNCTION_TYPE_1D::operator=(
            Function&&
        ) noexcept = default;

        // Access methods

        FUNCTION_TEMPLATE_1D
        ResultT FUNCTION_TYPE_1D::operator()(ArgT x) const {
            if (!_func) {
                throw std::runtime_error(
                    "Function::operator(): Function not initialized"
                );
            }

            return _func(x);
        }

        FUNCTION_TEMPLATE_1D
        ResultT FUNCTION_TYPE_1D::at(ArgT x) const {
            return (*this)(x);
        }

        // Static methods

        FUNCTION_TEMPLATE_1D
        FUNCTION_TYPE_1D FUNCTION_TYPE_1D::constant(ResultT value) {
            return Function([value](ArgT x) -> ResultT { return value; });
        }

        FUNCTION_TEMPLATE_1D
        FUNCTION_TYPE_1D FUNCTION_TYPE_1D::from_function_ptr(
            ResultT (*func)(ArgT)
        ) {
            return Function(std::function<ResultT(ArgT)>(func));
        }

        // Function 2-d class

        // Constructors / Destructor

        FUNCTION_TEMPLATE_2D
        FUNCTION_TYPE_2D::Function() = default;

        FUNCTION_TEMPLATE_2D
        FUNCTION_TYPE_2D::Function(
            std::function<ResultT(ArgT1, ArgT2)> func
        ) : _func(std::move(func)) {}

        FUNCTION_TEMPLATE_2D
        FUNCTION_TYPE_2D::~Function() = default;

        // Getter

        FUNCTION_TEMPLATE_2D
        bool FUNCTION_TYPE_2D::is_initialized() const noexcept {
            return static_cast<bool>(_func);
        }

        // Copy / Move operators

        FUNCTION_TEMPLATE_2D
        FUNCTION_TYPE_2D::Function(const Function&) = default;

        FUNCTION_TEMPLATE_2D
        FUNCTION_TYPE_2D::Function(Function&&) noexcept = default;

        FUNCTION_TEMPLATE_2D
        FUNCTION_TYPE_2D& FUNCTION_TYPE_2D::operator=(const Function&) = default;

        FUNCTION_TEMPLATE_2D
        FUNCTION_TYPE_2D& FUNCTION_TYPE_2D::operator=(
            Function&&
        ) noexcept = default;

        // Access methods

        FUNCTION_TEMPLATE_2D
        ResultT FUNCTION_TYPE_2D::operator()(ArgT1 x, ArgT2 y) const {
            if (!_func) {
                throw std::runtime_error(
                    "Function::operator(): Function not initialized"
                );
            }

            return _func(x, y);
        }

        FUNCTION_TEMPLATE_2D
        ResultT FUNCTION_TYPE_2D::at(ArgT1 x, ArgT2 y) const {
            return (*this)(x, y);
        }

        // Static methods

        FUNCTION_TEMPLATE_2D
        FUNCTION_TYPE_2D FUNCTION_TYPE_2D::constant(ResultT value) {
            return Function([value](ArgT1 x, ArgT2 y) -> ResultT { return value; });
        }

        FUNCTION_TEMPLATE_2D
        FUNCTION_TYPE_2D FUNCTION_TYPE_2D::from_function_ptr(
            ResultT (*func)(ArgT1, ArgT2)
        ) {
            return Function(std::function<ResultT(ArgT1, ArgT2)>(func));
        }

        // Function 3-d class

        // Constructors / Destructor

        FUNCTION_TEMPLATE_3D
        FUNCTION_TYPE_3D::Function() = default;

        FUNCTION_TEMPLATE_3D
        FUNCTION_TYPE_3D::Function(
            std::function<ResultT(ArgT1, ArgT2, ArgT3)> func
        ) : _func(std::move(func)) {}

        FUNCTION_TEMPLATE_3D
        FUNCTION_TYPE_3D::~Function() = default;

        // Getter

        FUNCTION_TEMPLATE_3D
        bool FUNCTION_TYPE_3D::is_initialized() const noexcept {
            return static_cast<bool>(_func);
        }

        // Copy / Move operators

        FUNCTION_TEMPLATE_3D
        FUNCTION_TYPE_3D::Function(const Function&) = default;

        FUNCTION_TEMPLATE_3D
        FUNCTION_TYPE_3D::Function(Function&&) noexcept = default;

        FUNCTION_TEMPLATE_3D
        FUNCTION_TYPE_3D& FUNCTION_TYPE_3D::operator=(const Function&) = default;

        FUNCTION_TEMPLATE_3D
        FUNCTION_TYPE_3D& FUNCTION_TYPE_3D::operator=(
            Function&&
        ) noexcept = default;

        // Access methods

        FUNCTION_TEMPLATE_3D
        ResultT FUNCTION_TYPE_3D::operator()(ArgT1 x, ArgT2 y, ArgT3 z) const {
            if (!_func) {
                throw std::runtime_error(
                    "Function::operator(): Function not initialized"
                );
            }

            return _func(x, y, z);
        }

        FUNCTION_TEMPLATE_3D
        ResultT FUNCTION_TYPE_3D::at(ArgT1 x, ArgT2 y, ArgT3 z) const {
            return (*this)(x, y, z);
        }

        // Static methods

        FUNCTION_TEMPLATE_3D
        FUNCTION_TYPE_3D FUNCTION_TYPE_3D::constant(ResultT value) {
            return Function(
                [value](ArgT1 x, ArgT2 y, ArgT3 z) -> ResultT { return value; }
            );
        }

        FUNCTION_TEMPLATE_3D
        FUNCTION_TYPE_3D FUNCTION_TYPE_3D::from_function_ptr(
            ResultT (*func)(ArgT1, ArgT2, ArgT3)
        ) {
            return Function(std::function<ResultT(ArgT1, ArgT2, ArgT3)>(func));
        }

        // Undef for internal-use only

        #undef FUNCTION_TEMPLATE_1D
        #undef FUNCTION_TEMPLATE_2D
        #undef FUNCTION_TEMPLATE_3D
        #undef FUNCTION_TYPE_1D
        #undef FUNCTION_TYPE_2D
        #undef FUNCTION_TYPE_3D
    }
}
