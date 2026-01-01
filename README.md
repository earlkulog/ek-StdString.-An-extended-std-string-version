# ek-StdString.-An-extended-std-string-version
This is an attempt to add features/functionalities to std::string

**Overview**

`ek::StdString` is a subclass of `std::string` designed to simplify integration with Windows APIs. Many Windows functions require string pointers (`LPCSTR` or `LPCWSTR`), which typically forces developers to call `std::string::c_str()`. This library eliminates that friction by providing automatic conversions, making API calls more convenient and reducing boilerplate code.

**Background**

The motivation behind creating `ek::StdString` was convenience. Initially, it was a simple utility to auto-convert `std::string`/`std::wstring` into `LPCSTR`/`LPCWSTR`. Over time, as new necessities arose, additional features and functionalities were added to extend its usability beyond basic conversions.

---

**Versions**

- `ek::StdStringA` – ANSI version  
- `ek::StdStringW` – Wide character version  
- `ek::StdString` – Uses either ANSI or wide character depending on project settings  

---

**Operators**

- **Conversion Operators**  
  - Automatic conversion from `std::string`/`std::wstring` to `const char*` / `const wchar_t*`.

- **Increment Operators (`++`)**  
  - In-place manipulation of the string.  
  - Example:
    ```cpp
    ek::StdString str = _TEXT("The quick brown fox jumps over the lazy dog.");
    ++str; // becomes "he quick brown fox jumps over the lazy dog."
    ek::StdString str2 = str++; 
    // str2 = "The quick brown fox jumps over the lazy dog."
    // str  = "he quick brown fox jumps over the lazy dog."
    ```

- **Decrement Operators (`--`)**  
  - Works the same as `++` but operates from the end of the string.

- **Natural std::string Operators**  
  - All standard `std::string` operators are preserved (or must be preserved if not explicitly overridden).

---

**Functions**

- **Trimming**
  - `ltrim()`, `rtrim()`, `trim()`  
    Removes whitespace at the start, end, or both ends. Returns a new instance.

- **Splitting**
  - `std::vector<t_std_str> split(_chr_type separator, int limit=-1)`  
    Works like JavaScript’s `String.prototype.split()`.  
    - Separator can be `char`, `wchar_t`, `const char*`, or `const wchar_t*`.  
    - Returns `std::vector<ek::StdStringA/W>`.

- **Joining**
  - `static t_std_str join(std::vector<t_std_str> a, const_type sep, int offset=0)`  
    Concatenates a given vector into a new `ek::StdString` instance.

- **Search**
  - `bool includes(const_type* a)`  
    Checks whether the given string is present. Case-sensitive.  
    - A case-insensitive version may be added.

- **Escaping**
  - `t_std_str escape(_chr_type quo)`  
    Returns a new escaped string.  
  - `t_std_str escape_ip(_chr_type quo)`  
    Escapes the string in place.

- **Formatting**
  - `static t_std_str format(const_type* fmt, ...)`  
    Works like `sprintf_s` / `swprintf_s`.  
    Currently supports format specifiers:  
    `c C d i o u x X e E f F g G a A n p s S Z`.

---

**Usage Example**

```cpp
#include "StdString.h"
#include <Windows.h>

int main() {
    ek::StdString s = "Hello, World!";
    MessageBoxA(NULL, s, "Title", MB_OK); // No need for c_str()
    return 0;
}
