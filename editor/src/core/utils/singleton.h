#pragma once

template <typename T> class Singleton {
  public:
    static T* Get();
    Singleton(const Singleton&)           = delete;
    Singleton& operator=(const Singleton) = delete;

  protected:
    struct singleton {};
    Singleton() = default;
};

template <typename T>

T* Singleton<T>::Get() {
    static T* instance = new T(singleton{});
    return instance;
}
