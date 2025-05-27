#ifndef LYRA_SHADER_WIN_COM_PTR_H
#define LYRA_SHADER_WIN_COM_PTR_H

template <class T>
class ComPtr
{
private:
    T* ptr;

public:
    ComPtr() : ptr(nullptr) {}

    explicit ComPtr(T* p) : ptr(p) {}

    ~ComPtr()
    {
        if (ptr != nullptr) {
            ptr->Release();
            ptr = nullptr;
        }
    }

    ComPtr(const ComPtr<T>& other) : ptr(other.ptr)
    {
        if (ptr != nullptr) {
            ptr->AddRef();
        }
    }

    ComPtr<T>& operator=(const ComPtr<T>& other)
    {
        if (this != &other) {
            if (ptr != nullptr) {
                ptr->Release();
            }
            ptr = other.ptr;
            if (ptr != nullptr) {
                ptr->AddRef();
            }
        }
        return *this;
    }

    T* Detach()
    {
        T* detached = ptr;
        ptr         = nullptr;
        return detached;
    }

    T** GetAddressOf()
    {
        return &ptr;
    }

    T* operator->() const
    {
        return ptr;
    }

    T** operator&()
    {
        return &ptr;
    }

    operator T*() const
    {
        return ptr;
    }

    bool operator!() const
    {
        return ptr == nullptr;
    }

    T* get() const
    {
        return ptr;
    }

    void reset(T* p = nullptr)
    {
        if (ptr != nullptr) {
            ptr->Release();
        }
        ptr = p;
    }
};

#endif // LYRA_SHADER_WIN_COM_PTR_H
