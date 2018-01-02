#ifndef MY_GUI_DATA_BINDING_H
#define MY_GUI_DATA_BINDING_H

namespace slt {

  template<typename T> 
  class ValueBinding {
  public:
    virtual ~ValueBinding() {}

    virtual T get() const = 0;
  };

  template<typename T> 
  class ConstantValueBinding : public ValueBinding <T>{
  public:
    ConstantValueBinding(T v) 
     : val_(std::move(v)) {}

    T get() const { return val_; }
   private:
    T val_;
  };

  template<typename T> 
  class PointerValueBinding : public ValueBinding <T> {
  public:
    PointerValueBinding(T* p)
     : ptr_(p) {}

    T get() const { return *ptr_; }

   private:
    T* ptr_;
  };

  template<typename T>
  class BindableValue {
  public:
    BindableValue()
      : bound_(std::make_unique<ConstantValueBinding<T>>(T())) {}

    BindableValue(T value)
      : bound_(std::make_unique<ConstantValueBinding<T>>(std::move(value))) {}

    BindableValue(T* value_link)
      : bound_(std::make_unique<PointerValueBinding<T>>(value_link)) {}

    T get() const {
      return bound_->get();
    }

    void set(std::unique_ptr<ValueBinding<T>> b) {
      bound_ = std::move(b);
    }
  private:
    std::unique_ptr<ValueBinding<T>> bound_;
  };
}

#endif