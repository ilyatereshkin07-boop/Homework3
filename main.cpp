#include <iostream>
#include <string>
#include <string_view>
#include <random>
#include <format>
#include <stdexcept>
#include <cmath>
#include <vector>
#include <memory>


namespace Quantities {

    
    class Ampere {
        double value_;
    public:
        explicit Ampere(double v) : value_(v) {}
        double value() const { return value_; }
    };

    class Volt {
        double value_;
    public:
        explicit Volt(double v) : value_(v) {}
        double value() const { return value_; }
    };

    class Ohm {
        double value_;
    public:
        explicit Ohm(double v) : value_(v) {}
        double value() const { return value_; }
    };

    class Joule {
        double value_;
    public:
        explicit Joule(double v) : value_(v) {}
        double value() const { return value_; }
    };

    class Watt {
        double value_;
    public:
        explicit Watt(double v) : value_(v) {}
        double value() const { return value_; }
    };

    class Second {
        double value_;
    public:
        explicit Second(double v) : value_(v) {}
        double value() const { return value_; }
    };

    
    namespace Literals {
        Ampere operator"" _mA(long double val) { return Ampere(static_cast<double>(val) / 1000.0); }
        Volt operator"" _V(long double val) { return Volt(static_cast<double>(val)); }
        Ohm operator"" _Ohm(long double val) { return Ohm(static_cast<double>(val)); }
        Joule operator"" _J(long double val) { return Joule(static_cast<double>(val)); }
        Watt operator"" _W(long double val) { return Watt(static_cast<double>(val)); }
        Second operator"" _s(long double val) { return Second(static_cast<double>(val)); }
    }

   
    Volt operator*(const Ampere& i, const Ohm& r) {
        return Volt(i.value() * r.value());
    }
    Volt operator*(const Ohm& r, const Ampere& i) {
        return Volt(i.value() * r.value());
    }

    
    Watt operator*(const Volt& v, const Ampere& i) {
        return Watt(v.value() * i.value());
    }
    Watt operator*(const Ampere& i, const Volt& v) {
        return Watt(v.value() * i.value());
    }

    
    Joule operator*(const Watt& w, const Second& s) {
        return Joule(w.value() * s.value());
    }
    Joule operator*(const Second& s, const Watt& w) {
        return Joule(w.value() * s.value());
    }

    
    Ohm operator/(const Volt& v, const Ampere& i) {
        if (i.value() == 0.0) throw std::domain_error("Деление на ноль (ток)");
        return Ohm(v.value() / i.value());
    }

    
    Ampere operator/(const Volt& v, const Ohm& r) {
        if (r.value() == 0.0) throw std::domain_error("Деление на ноль (сопротивление)");
        return Ampere(v.value() / r.value());
    }

} 

using namespace Quantities::Literals;
using namespace Quantities;


template <typename T>
class Measurement {
    double time_;      
    T value_;          
    double error_;    

public:
    Measurement(double time, T value, double error)
        : time_(time), value_(value), error_(error) {
        
        if (time_ < 0) throw std::invalid_argument("Время не может быть отрицательным");
        if (error_ < 0) throw std::invalid_argument("Погрешность не может быть отрицательной");
    }

    double time() const { return time_; }
    T value() const { return value_; }
    double error() const { return error_; }
};


template <typename T>
struct std::formatter<Measurement<T>> {
    constexpr auto parse(std::format_parse_context& ctx) {
        return ctx.begin();
    }

    auto format(const Measurement<T>& m, std::format_context& ctx) const {
        
        return std::format_to(ctx.out(), "Measurement(value={:.4f}, error={:.4f}, time={:.2f}s)",
            m.value().value(), m.error(), m.time());
    }
};


namespace Instruments {

    
    template <typename T>
    class GenericDevice {
    protected:
        double min_range_;
        double max_range_;
        int accuracy_class_; 
        std::string id_;

        
        double random_value() const {
            static std::random_device rd;
            static std::mt19937 gen(rd());
            std::uniform_real_distribution<> dis(min_range_, max_range_);
            return dis(gen);
        }

        
        double calculate_error() const {
            return (accuracy_class_ / 100.0) * max_range_;
        }

    public:
        GenericDevice(double min_r, double max_r, int acc_class, std::string id)
            : min_range_(min_r), max_range_(max_r), accuracy_class_(acc_class), id_(std::move(id)) {
            
            if (min_r >= max_r) throw std::invalid_argument("Мин. диапазон должен быть меньше макс.");
            if (acc_class <= 0) throw std::invalid_argument("Класс точности должен быть положительным");
        }
        virtual ~GenericDevice() = default;

        
        GenericDevice(const GenericDevice&) = delete;
        GenericDevice& operator=(const GenericDevice&) = delete;

       
        Measurement<T> measure() {
            double val = random_value();
            double err = calculate_error();
            double time = 0.1 + (rand() % 90) / 100.0; 
            return Measurement<T>(time, T(val), err);
        }
    };

   
    class Voltmeter : public GenericDevice<Volt> {
    public:
        Voltmeter(double min_r, double max_r, int acc_class, std::string id)
            : GenericDevice<Volt>(min_r, max_r, acc_class, std::move(id)) {}
    };

    class Ammeter : public GenericDevice<Ampere> {
    public:
        Ammeter(double min_r, double max_r, int acc_class, std::string id)
            : GenericDevice<Ampere>(min_r, max_r, acc_class, std::move(id)) {}
    };

    template <typename T>
    class Multimeter : public GenericDevice<T> {
    public:
        Multimeter(double min_r, double max_r, int acc_class, std::string id)
            : GenericDevice<T>(min_r, max_r, acc_class, std::move(id)) {}
    };

} 
void demonstrate_quantities() {
    std::cout << "--- Демонстрация физических величин ---\n";

    auto current = 2.0_mA;
    auto resistance = 100.0_Ohm;
    auto voltage = current * resistance; 
    auto power = voltage * current;
    auto time = 5.0_s;
    auto energy = power * time;

    std::cout << "Ток: " << current.value() << " А\n";
    std::cout << "Сопротивление: " << resistance.value() << " Ом\n";
    std::cout << "Напряжение (I*R): " << voltage.value() << " В\n";
    std::cout << "Мощность (V*I): " << power.value() << " Вт\n";
    std::cout << "Энергия (P*t): " << energy.value() << " Дж\n";

    auto calc_current = voltage / resistance;
    std::cout << "Ток (V/R): " << calc_current.value() << " А\n";
}

void demonstrate_devices() {
    std::cout << "\n--- Демонстрация приборов ---\n";

    Instruments::Voltmeter v(0.0, 220.0, 1, "V001");
    Instruments::Ammeter a(0.0, 10.0, 2, "A001");
    Instruments::Multimeter<Volt> m(0.0, 500.0, 1, "M001");

    auto res_v = v.measure();
    auto res_a = a.measure();
    auto res_m = m.measure();

    
    std::cout << std::format("Вольтметр: {}\n", res_v);
    std::cout << std::format("Амперметр: {}\n", res_a);
    std::cout << std::format("Мультиметр: {}\n", res_m);
}

int main() {
    setlocale(LC_ALL, "Russian");

    demonstrate_quantities();
    demonstrate_devices();

    return 0;
} 
