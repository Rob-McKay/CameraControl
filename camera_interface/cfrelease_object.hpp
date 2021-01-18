#pragma once

#ifdef __MACOS__

template <typename ref_obj> class cfrelease_object
{
    ref_obj ref;

public:
    cfrelease_object(ref_obj ref)
        : ref(ref)
    {
    }

    cfrelease_object(const cfrelease_object<ref_obj>& other) = delete;
    //     : ref(other.ref)
    // {
    // }

    cfrelease_object(cfrelease_object<ref_obj>&& other) = delete; // { std::swap(ref, other.ref); }

    cfrelease_object& operator=(const cfrelease_object<ref_obj>& other) = delete;
    cfrelease_object& operator=(cfrelease_object<ref_obj>&& other) = delete;
    // {
    //     std::swap(ref, other.ref);
    //     return *this;
    // }

    ~cfrelease_object() { CFRelease(ref); }

    const ref_obj& get_obj() const { return ref; }
};
#endif
