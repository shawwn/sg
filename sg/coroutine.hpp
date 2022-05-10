#pragma once

#include <cassert>
#include <concepts>
#include <coroutine>
#include <exception>
#include <iostream>

namespace sg::coroutine_detail {
template <class From, class To>
concept Convertible = std::is_convertible_v<From, To>;
}

namespace sg {

template <typename T>
struct Generator {
  using value_type = T;

  // https://stackoverflow.com/questions/8164567/how-to-make-my-custom-type-to-work-with-range-based-for-loops
  struct iterator {
    using Gen = Generator<T>;

    iterator(const Gen* self) : self(const_cast<Gen*>(self)) {}

    iterator operator++() {
      if (self) {
        ++(*self);
      }
      return *this;
    }

    bool operator!=(const iterator& other) const {
      if (self && !other.self) {
        if (!*self) {
          return false;
        }
        return true;
      }
      return self != other.self;
    }

    auto& operator*() { return **self; }
    const auto& operator*() const { return **self; }

  protected:
    Gen* self;
  };

  iterator begin() const { return iterator(this); }

  iterator end() const { return iterator(nullptr); }

  // The class name 'Generator' is our choice and
  // it is not required for coroutine magic.
  // Compiler recognizes coroutine by the presence of 'co_yield' keyword.
  // You can use name 'MyGenerator' (or any other name) instead
  // as long as you include nested struct promise_type
  // with 'MyGenerator get_return_object()' method .
  //(Note:You need to adjust class constructor/destructor names too when
  // choosing to rename class)

  struct promise_type;
  using handle_t = std::coroutine_handle<promise_type>;

  struct promise_type {
    T value_;
    std::exception_ptr exception_;

    Generator get_return_object() {
      return Generator(handle_t::from_promise(*this));
    }

    using suspend_t = std::suspend_always;

    suspend_t initial_suspend() { return {}; }

    suspend_t final_suspend() noexcept { return {}; }

    void unhandled_exception() {
      // saving exception
      exception_ = std::current_exception();
    }

    suspend_t yield_value(::sg::coroutine_detail::Convertible<T> auto&& from) {
      // caching the result in promise
      value_ = std::forward<decltype(from)>(from);
      return {};
    }

    void return_void() {}
  };

  handle_t h_;

  Generator() = default;

  Generator(Generator<T>&& other)
      : h_(std::exchange(other.h_, nullptr)),
        full_(std::exchange(other.full_, false)) {}

  Generator(handle_t&& h) : h_(std::move(h)) {}
  Generator(const handle_t& h) : h_(h) {}

  ~Generator() {
    if (h_)
      h_.destroy();
  }

  explicit operator bool() const {
    fill();
    // The only way to reliably find out whether or not we finished coroutine,
    // whether or not there is going to be a next value generated (co_yield) in
    // coroutine via C++ getter (operator () below) is to execute/resume
    // coroutine until the next co_yield point (or let it fall off end). Then we
    // store/cache result in promise to allow getter (operator() below to grab
    // it without executing coroutine)
    return h_ && !h_.done();
  }

  Generator<T>& operator++() {
    full_ = false;
    fill();
    return *this;
  }

  T& operator*() const {
    assert(h_);
    assert(full_);
    return h_.promise().value_;
  }

protected:
  mutable bool full_ = false;

  void fill() const {
    if (h_ && !full_) {
      h_();
      if (h_.promise().exception_)
        std::rethrow_exception(h_.promise().exception_);
      // propagate coroutine exception in called context

      full_ = true;
    }
  }
};

} // namespace sg