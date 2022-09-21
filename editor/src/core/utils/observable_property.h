#pragma once

#include <vector>
#include <functional>
#include <memory>

using ObservableSubscriberHandle = unsigned int;

template<typename T>
class ObservableProperty {
  public:
    struct Subscriber {
        ObservableSubscriberHandle handle = 0;
        std::function<void(const T&, const T&)> onValueChangedFunc = nullptr;
    };

    ObservableProperty() = default;
    ObservableProperty(T val) : value(val) {}

    static std::shared_ptr<T> Create(T val = T{}) {
        return std::make_shared<T>(val);
    }

    ObservableSubscriberHandle RegisterSubscriber(std::function<void(const T& newValue, const T& oldValue)> onValueChangedFunc) {
        const Subscriber newSub = { subscriberIndex++, onValueChangedFunc };
        subscribers.emplace_back(newSub);
        return newSub.handle;
    }

    void UnregisterSubscriber(ObservableSubscriberHandle handle) {
        subscribers.erase(std::remove_if(subscribers.begin(), subscribers.end(), [handle](const Subscriber& sub) {
            return handle == sub.handle;
        }), subscribers.end());
    }

    void SetValue(const T& newValue) {
        if (value != newValue) {
            T oldValue = value;
            value = newValue;
            for (auto& sub : subscribers) {
                sub.onValueChangedFunc(value, oldValue);
            }
        }
    }

    [[nodiscard]] T GetValue() const {
        return value;
    }

    void ClearSubscribers() {
        subscribers.clear();
        subscriberIndex = 1;
    }

    unsigned int subscriberIndex = 1;
    std::vector<Subscriber> subscribers{};

  private:
    T value{};
};
