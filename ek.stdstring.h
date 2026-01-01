#pragma once

#include <tchar.h>
#include <string>
#include <vector>
#include <algorithm>
#include <cwctype>

namespace ek {
	template<class _chr_type>
	struct t_std_str : std::basic_string<_chr_type> {
		using base_type = std::basic_string<_chr_type>;
		using base_type::basic_string;
		using const_type = const _chr_type;
		//using base_type::operator[];
		t_std_str(const_type* a) : base_type(a) {}
		t_std_str& operator=(const_type* a) {
			base_type::operator=(a);
			return *this;
		}
		t_std_str(const base_type& other) : base_type(other) {}

#if defined(CEF_INCLUDE_INTERNAL_CEF_STRING_H_)
		t_std_str(const CefString& str) : base_type(utils<_chr_type>::cef_str_to_str(str)) {
		}

		t_std_str& operator = (const CefString& src) {
			base_type::operator=(utils<_chr_type>::cef_str_to_str(src));
			return *this;
		}
#endif

		t_std_str& operator=(const base_type& other) {
			base_type::operator=(other);
			return *this;
		}
		_chr_type operator[](int pos) const {
			return base_type::operator[](pos);
		}
		static t_std_str format(const_type* fmt, ...) {
			t_std_str str;
			va_list va;
			va_start(va, fmt);
			parse_formatted(str, fmt, va);
			return str;
		}
		static t_std_str join(std::vector<t_std_str> a, const_type sep, int offset=0) {
			auto ch = a.size();
			t_std_str res;
			for (uint32_t i = offset; i < ch; i++) {
				if (i > offset)
					res += sep;
				res += a[i];
			}
			return res;
		}

		operator const_type* () const {
			return this->c_str();
		}

		t_std_str operator++() {
			auto ptr = this->c_str();
			this->assign(++ptr);
			return *this;
		}

		t_std_str& operator++(int) {
			auto ptr = this->c_str();
			t_std_str ret = ptr;
			this->assign(++ptr);
			return ret;
		}

		t_std_str operator--() {
			t_std_str ret = this;
			auto ptr = this->c_str();
			this->assign(ptr, this->size() - 1);
			return ret;
		}

		t_std_str& operator--(int) {
			auto ptr = this->c_str();
			this->assign(ptr, this->size() - 1);
			return *this;
		}

		bool includes(const_type* a) {
			return this->find(a) != base_type::npos;
		}

	private:
		static int _ltrim(const_type* a) {
			int ret = 0;
			while (*a && utils<_chr_type>::is_space(*a++))
				ret++;
			return ret;
		}
		static int _rtrim(const_type* a, int size) {
			if (size-- == 0)return 0;
			a += size;
			while (size >= 0 && utils<_chr_type>::is_space(*a--))
				size--;
			return size + 1;
		}

	public:

		t_std_str ltrim() {
			int a = _ltrim(*this);
			return this->substr(a);
		}

		t_std_str rtrim() {
			int a = _rtrim(*this, static_cast<int>(this->size()));
			return this->substr(0, a);
		}

		t_std_str trim() {
			int a = _ltrim(*this);
			int b = _rtrim(*this, static_cast<int>(this->size()));
			return this->substr(a, b - a);
		}

		/*t_std_str& operator += (const_type* a) {
			return base_type::operator+=(a);
		}

		t_std_str& operator += (t_std_str a) {
			return base_type::operator+=(a);
		}

		t_std_str operator+=(uint32_t pos) {
			auto ptr = this->c_str();
			uint32_t ch = static_cast<uint32_t>(this->size());
			if (ch == 0)
				return t_std_str();
			pos = min(pos, ch - 1);
			return t_std_str(ptr, pos);
		}

		t_std_str& operator += (const_type a) {
			_chr_type b[] = { a,0 };
			this->append(b);
			return *this;
		}

		t_std_str& operator += (const t_std_str& a) {
			this->append(a);
			return *this;
		}*/
		t_std_str escape(_chr_type quo) {
			t_std_str ret = *this;
			ret.escape_ip(quo);
			return ret;
		}

		t_std_str& escape_ip(_chr_type quo) {
			uint32_t last = 0;

			t_std_str temp;
			temp += quo;
			for (auto chr : *this) {
				switch (chr) {
				case '\\':
				case '\t':
				case '\n':
				case '\r':
					temp += '\\';
					break;
				case '\'':
					if (quo == '\'')
						temp += '\\';
					break;
				case '"':
					if(quo=='"')
						temp += '\\';
					break;
				}
				temp += chr;
			}
			temp += quo;
			this->assign(temp);
			return *this;
		}

		std::vector<t_std_str> split(_chr_type separator, int limit=-1) {
			_chr_type sep[] = { separator, 0 };
			return split(sep, true, limit);
		}

		std::vector<t_std_str> split(const_type* separator, bool caseSensitive=false, int limit=-1) {
			std::vector<t_std_str> result;
			uint32_t count = 0, max = static_cast<uint32_t>(limit);
			auto haystack = this->c_str();
			int ch = base_type(separator).size();
			while (1) {
				uint32_t pos;
				if (caseSensitive) {
					t_std_str hs(haystack);
					auto it = hs.find(separator);
					if (it == base_type::npos)
						break;
					pos = it;
				}
				else {
					auto it = std::search(
						haystack, haystack + std::strlen(haystack),
						separator, separator + std::strlen(separator),
						ci_comparer
					);
					if (!*it)break;
					pos = it - haystack;
				}
				result.push_back(t_std_str(haystack, pos));
				pos += ch;
				haystack += pos;
				if (++count == max)break;
			}
			if(*haystack)
				result.push_back(t_std_str(haystack));
			return result;
		}

	private:
		template<class T>
		struct utils;

		template<>
		struct utils<char> {
			inline static const char* fmt_specs = "cCdiouxXeEfFgGaAnpsSZ";
			static const char* _tpbrk(const char* str) {
				return strpbrk(str, fmt_specs);
			}
			static int printf(char* str, const char* fmt, va_list va) {
				return vsprintf_s(str, 1024, fmt, va);
			}
			static bool is_space(unsigned char ch) {
				return std::isspace(ch);
			}
#if defined(CEF_INCLUDE_INTERNAL_CEF_STRING_H_)
			static std::string cef_str_to_str(const CefString& src) {
				std::string a = src;
				return a;
			}
#endif
		};

		template<>
		struct utils<wchar_t> {
			inline static const wchar_t* fmt_specs = L"cCdiouxXeEfFgGaAnpsSZ";
			static const wchar_t* _tpbrk(const wchar_t* str) {
				return wcspbrk(str, fmt_specs);
			}
			static int printf(wchar_t* str, const wchar_t* fmt, va_list va) {
				return vswprintf_s(str, 2024, fmt, va);
			}
			static bool is_space(wchar_t ch) {
				return std::iswspace(ch);
			}
#if defined(CEF_INCLUDE_INTERNAL_CEF_STRING_H_)
			static std::wstring cef_str_to_str(const CefString& src) {
				std::wstring a = src;
				return a;
			}
#endif
		};

		static bool ci_comparer(const_type a, const_type b) {
			return std::tolower(static_cast<const_type>(a)) ==
				std::tolower(static_cast<const_type>(b));
		}

		static void parse_formatted(t_std_str& out, const_type* fmt, va_list va) {
			while (1) {
				t_std_str _fmt = fmt;
				auto pos = _fmt.find("%");
				if (pos != base_type::npos) {
					out.append(fmt, pos);
					fmt += pos;
					if (!(fmt + 1))
						break;
					fmt++;
					if (*fmt == '%') {
						out += '%';
						fmt++;
					}
					else {
						const_type* spec = utils<_chr_type>::_tpbrk(fmt);
						if (spec) {
							_chr_type buf[1024] = {};
							_chr_type _f[] = { '%',0 };
							base_type f = _f;
							f.append(fmt,spec - fmt);
							f += *spec;
							va_list _va = va;
							utils<_chr_type>::printf(buf, f.c_str(), _va);
							out += buf;
							fmt = spec + 1;
							switch (*spec) {
							case 'c':
							case 'C':
								move_arg<char>(va);
								break;
							case 'd':
							case 'i':
							case 'o':
							case 'u':
							case 'x':
							case 'X':
								move_arg<int>(va);
								break;
							case 'S':
								if (sizeof(_chr_type) == 1)
									move_str_arg<wchar_t>(va);
								else
									move_str_arg<char>(va);
								break;
							case 's':
								move_str_arg<_chr_type>(va);
								break;
							case 'Z':
								move_str_arg<wchar_t>(va);
								break;
							case 'e': case 'E': case 'f': case 'F':
							case 'g': case 'G': case 'a': case 'A':
								move_arg<double>(va);
								break;
							case 'n':
								move_arg<int*>(va);
								break;
							case 'p':
								move_arg<void*>(va);
								break;
							}
						}
						else {
							out.append(fmt);
							break;
						}
					}
				}
				else {
					if (fmt)
						out.append(fmt);
					break;
				}
			}
			va_end(va);
		}
		template<class T>
		static void move_arg(va_list& ap) {
			ap += _INTSIZEOF(T);
		}
		template<class T>
		static void move_str_arg(va_list& ap) {
			move_arg<T*>(ap);
		}
	};

	template<class _chr_type>
	t_std_str<_chr_type> operator+(t_std_str<_chr_type> a, const _chr_type* b) {
		a += b;
		return a;
	}

	template<class _chr_type>
	t_std_str<_chr_type>& operator+=(t_std_str<_chr_type>& a, const _chr_type* b) {
		a.append(b);
		return a;
	}

	template<class _chr_type>
	t_std_str<_chr_type> operator+(t_std_str<_chr_type> a, const t_std_str<_chr_type>& b) {
		a += b;
		return a;
	}

	template<class _chr_type>
	t_std_str<_chr_type>& operator+=(t_std_str<_chr_type>& a, const t_std_str<_chr_type>& b) {
		a.append(b);
		return a;
	}

	template<class _chr_type>
	t_std_str<_chr_type> operator+(t_std_str<_chr_type> a, _chr_type b) {
		_chr_type c[] = { b, 0 };
		return a + c;
	}

	template<class _chr_type>
	t_std_str<_chr_type>& operator+=(t_std_str<_chr_type>& a, _chr_type b) {
		_chr_type c[] = { b, 0 };
		a += c;
		return a;
	}

	template<class _chr_type>
	t_std_str<_chr_type> operator+(const _chr_type* a, t_std_str<_chr_type> b) {
		t_std_str<_chr_type> c;
		c = a;
		return c + b;
	}

	using StdStringW = t_std_str<wchar_t>;
	using StdStringA = t_std_str<char>;

#if defined(UNICODE) || defined(_UNICODE)
	using StdString = StdStringW;
#else
	using StdString = StdStringA;
#endif
}