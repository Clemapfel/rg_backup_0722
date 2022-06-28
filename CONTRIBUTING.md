never use class, always struct
## never use .cpp files

The .hpp files should include all declarations for a specific class or namespace. The definitions of all functions 
declared in that file (that were not declared `extern`) should be in an `.inl` file of the same name (for example 
`include/class_name.hpp` and `.src/class_name.inl`. At the end of `include/class_name.cpp` should be an `#include <.src/class_name.inl>`.

This way of structuring a library is called "header only" and should be the default, as it allows easier linking and
intergration 

## always use `,` in multi-line declarations of the same type

If multiple variables of the same type are declared in the same scope, at the same, a `,` should be used and
all declarations shall be indented. Example:

```cpp
// WRONG: use , instead
size_t variable_a;
size_t variable_b;
int variable_c;

// WRONG: no indent
size_t variable_a, variable_b;
int variable_c;

// CORRECT:
size_t variable_a,
       variable_b;
int variable_c;
```

## never use `struct`

In no circumstances should the keyword `struct` be used. This is because, in Doxygen, `struct` and `class` have
different commands for generating documentation, making it so third parties will have to check the code
itself everytime, instead of just knowing he name of a class and calling `doxygenclass <name>`. Furthermore,
any member of a `class` is private by default. Using `struct` may accidentally expose private members to
users without any compiler warning. If, instead, a member of a `class` is erroneously not made `public`, a 
compiler error will be issued, notifying the user of the problem.

```cpp
// WRONG:
struct Type
{
    size_t field_a,
           field_b;
};

// CORRECT:
class Type
{
    public:
        size_t field_a,
               field_b;
};
```

## never use `var_args`

C-style variadic arguments shall never be used. Instead, use (in C++20) `template<Is<T>... Ts>`:

```cpp
// WRONG:
void variadic_function(size_t... args);

// CORRECT: 
template<typename T, typename U>
concept Is = std::is_same_v<T, U>;

template<Is<size_t>... size_ts>
void variadic_function(size_ts... args);

// CORRECT (but only in non-C++20):
template<typename... size_ts>
void variadic_function(size_ts... args);
```

The version utilizing `Is<T>` should be preferred, as calling the third version of `variadic_function` with a non-size_t
may result in a less-readable compiler error when compared to `concept`s.

## never throw an exception, never `exit` with anything but 0

No matter the type of project, it shall be impossible for a crash to occur under any circumstances. 
This includes any exit code other than 0 or any C-style error (such as `double free`, `SIGSEGV` or `SIGKILL`).
If the project is an application that ships compiled, it shall be furthermore impossible for an exception 
to trigger the end of runtime. If the project is a library aimed at other developers rather than end-users, 
forwarding of exceptions is acceptable.

```cpp
std::vector<size_t> storage = // ...

// WRONG: because this can trigger undefined behavior
size_t get_storage_element(size_t i) 
{
    return storage.data()[i];
}

// WRONG: because this can trigger and std::out_of_bounds exception
size_t get_storage_element(size_t i) 
{
    return storage.at(i);
}

// CORRECT: issues a soft warning instead
size_t get_storage_element(size_t i) 
{
    if (i > storage.size())
    {
        std::cerr << "[WARNING] In get_storage_element: index " << i 
                  << " is out of bounds for storage of size " << storage.size() 
                  << ". Returning size_t(-1) instead" << std::endl;
        
        return size_t(-1);
    }
    
    return storage.at(i);
}
```
