#pragma once

#include <experimental/coroutine>
#include <exception>
#include <iostream>
#include <concepts>

namespace sg {

    template<typename T>
    struct Generator {
        using value_type = T;
        // https://stackoverflow.com/questions/8164567/how-to-make-my-custom-type-to-work-with-range-based-for-loops
        class iterator {
        public:
            using Gen = Generator<T>;

            iterator(const Gen *self) : self(const_cast<Gen *>(self)) {}

            iterator operator++() {
              ++(*self);
              return *this;
            }

            bool operator!=(const iterator &other) const {
              if (self && !other.self) {
                if (!*self) {
                  return false;
                }
                return true;
              }
              return self != other.self;
            }

            auto operator*() const { return **self; }

        private:
            Gen *self;
        };

        iterator begin() const { return iterator(this); }

        iterator end() const { return iterator(nullptr); }

        // The class name 'Generator' is our choice and
        // it is not required for coroutine magic.
        // Compiler recognizes coroutine by the presence of 'co_yield' keyword.
        // You can use name 'MyGenerator' (or any other name) instead
        // as long as you include nested struct promise_type
        // with 'MyGenerator get_return_object()' method .
        //(Note:You need to adjust class constructor/destructor names too when choosing to rename class)

        struct promise_type;
        using handle_type = std::experimental::coroutine_handle<promise_type>;

        struct promise_type {// required
            T value_;
            std::exception_ptr exception_;

            Generator get_return_object() {
              return Generator(handle_type::from_promise(*this));
            }

            std::experimental::suspend_always initial_suspend() { return {}; }

            std::experimental::suspend_always final_suspend() noexcept { return {}; }

            void unhandled_exception() { exception_ = std::current_exception(); }//saving exception
//        template<convertible_to<T> From> // C++20 concept
            template<typename From>
            requires std::is_convertible_v<From, T>
            std::experimental::suspend_always yield_value(From &&from) {
//          static_assert(std::is_convertible_v<From, T>);
              value_ = std::forward<From>(from);//caching the result in promise
              return {};
            }

            void return_void() {}
        };

        handle_type h_;

        Generator() = default;

        Generator(Generator<T> &&other)
                : h_(std::exchange(other.h_, nullptr)), full_(std::exchange(other.full_, false)) {
        }

        Generator(handle_type h) : h_(h) {}

        ~Generator() { if (h_) h_.destroy(); }

        explicit operator bool() {
          fill();
          // The only way to reliably find out whether or not we finished coroutine,
          // whether or not there is going to be a next value generated (co_yield) in coroutine
          // via C++ getter (operator () below)
          // is to execute/resume coroutine until the next co_yield point (or let it fall off end).
          // Then we store/cache result in promise to allow getter (operator() below to grab it
          // without executing coroutine)
          return h_ && !h_.done();
        }

        Generator<T> &operator++() {
          full_ = false;
          fill();
          return *this;
        }

        T operator*() {
          assert(h_);
          assert(full_);
          return h_.promise().value_;
        }

        T operator()() {
          fill();
          full_ = false;//we are going to move out previously cached result to make promise empty again
          return std::move(h_.promise().value_);
        }

    private:
        bool full_ = false;

        void fill() {
          if (h_ && !full_) {
            h_();
            if (h_.promise().exception_)
              std::rethrow_exception(h_.promise().exception_);
            //propagate coroutine exception in called context

            full_ = true;
          }
        }
    };

//    extern Generator<uint64_t> fibonacci_sequence(unsigned n);
    extern auto fibonacci_sequence(auto n) -> Generator<decltype(n)>;

    extern void testCoroutine();
}