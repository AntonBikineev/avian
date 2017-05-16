#ifndef __JNI_SUPPORT_H__
#define __JNI_SUPPORT_H__

#include <unordered_map>

namespace jni_support {
class native_registry {
  public:
    static native_registry &get() noexcept {
        static native_registry registry;
        return registry;
    }

    void register_method(const char *name, void *ptr) {
        data_.emplace(std::make_pair(name, ptr));
    }

    void *get_method(const char *name) const {
        auto method = data_.find(name);
        if (method == data_.end())
            return nullptr;

        return method->second;
    }

  private:
    native_registry() noexcept = default;

    std::unordered_map<std::string, void *> data_;
};

template <class FunctionType, FunctionType *address> struct instantiate_method {
    instantiate_method(const char *name) {
        native_registry::get().register_method(
            name, reinterpret_cast<void *>(address));
    }
};
}

#define JNI_SUPPORT_STRINGIZE_(arg) #arg
#define JNI_SUPPORT_STRINGIZE(arg) JNI_SUPPORT_STRINGIZE_(arg)

#define JNI_SUPPORT_CONCAT_(x, y) x##y
#define JNI_SUPPORT_CONCAT(x, y) JNI_SUPPORT_CONCAT_(x, y)

#define JNI_SUPPORT_REGISTER(function_name)                                    \
    static ::jni_support::instantiate_method<decltype(function_name),          \
                                             &function_name>                   \
        JNI_SUPPORT_CONCAT(function_name, __LINE__) =                          \
            JNI_SUPPORT_STRINGIZE(function_name);                              \
/**/

#endif // __JNI_SUPPORT_H__
