#pragma once
#include <cxxabi.h>

#include <bitset>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <ostream>
#include <sstream>
#include <string>
#include <type_traits>
#include <typeinfo>
#if defined(_WIN32)
#include <windows.h>
typedef uintptr_t ptr_addr;
inline bool is_pointer_safe_to_read(ptr_addr ptr) {
  const void* non_volatile_ptr =
      const_cast<const void*>(reinterpret_cast<void*>(ptr));
  MEMORY_BASIC_INFORMATION mbi;
  if (!VirtualQuery(non_volatile_ptr, &mbi, sizeof(mbi))) return false;
  bool isCommitted = (mbi.State == MEM_COMMIT);
  bool isReadable =
      (mbi.Protect & (PAGE_READONLY | PAGE_READWRITE | PAGE_EXECUTE_READ |
                      PAGE_EXECUTE_READWRITE));
  bool isNotGuard = !(mbi.Protect & PAGE_GUARD);

  return isCommitted && isReadable && isNotGuard;
}
#elif defined(__linux__)
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
typedef uintptr_t ptr_addr;
struct MemoryRegion {
  uintptr_t start;
  uintptr_t end;
  bool readable;
};
inline std::vector<MemoryRegion> parse_proc_self_maps() {
  std::vector<MemoryRegion> regions;
  std::ifstream maps("/proc/self/maps");
  std::string line;
  while (std::getline(maps, line)) {
    std::istringstream iss(line);
    std::string addr_range, perms;
    if (!(iss >> addr_range >> perms)) continue;

    auto dash = addr_range.find('-');
    uintptr_t start = std::stoul(addr_range.substr(0, dash), nullptr, 16);
    uintptr_t end = std::stoul(addr_range.substr(dash + 1), nullptr, 16);

    bool readable = (perms.size() > 0 && perms[0] == 'r');

    regions.push_back({start, end, readable});
  }
  return regions;
}
inline bool is_pointer_safe_to_read(ptr_addr ptr) {
  static std::vector<MemoryRegion> regions = parse_proc_self_maps();
  uintptr_t addr = ptr;

  for (const auto& region : regions) {
    if (addr >= region.start && addr < region.end && region.readable) {
      return true;
    }
  }
  return false;
}
#elif defined(__APPLE__)
#include <mach/mach.h>

typedef unsigned long long ptr_addr;
inline bool is_pointer_safe_to_read(ptr_addr ptr) {
  mach_port_t task = mach_task_self();
  vm_address_t address = ptr;
  vm_size_t size = 0;
  vm_region_basic_info_data_64_t info;
  mach_msg_type_number_t info_count = VM_REGION_BASIC_INFO_COUNT_64;
  memory_object_name_t object;

  vm_address_t original_address = address;
  kern_return_t kr = vm_region_64(task, &address, &size, VM_REGION_BASIC_INFO_64,
                                    reinterpret_cast<vm_region_info_t>(&info), &info_count, &object);

  if (kr != KERN_SUCCESS) {
    return false;
  }

  // Check if the memory region is readable
  return ((info.protection & VM_PROT_READ) != 0) && (original_address >= address);
}
#else
#error "Platform not supported!"
#endif
#define analyzeType(var) analyzeType_impl<decltype(var)>(var)
template <typename T>
struct my_demangle {
  static std::string name() {
    int status = 0;
    char* dem =
        abi::__cxa_demangle(typeid(T).name(), nullptr, nullptr, &status);
    std::string s = (status == 0 ? dem : typeid(T).name());
    std::string cv = "";
    delete dem;
    if constexpr (std::is_rvalue_reference_v<T>) s += "&&";
    if constexpr (std::is_lvalue_reference_v<T>) s += "&";
    if constexpr (std::is_const_v<std::remove_reference_t<T>>) cv += " const";
    if constexpr (std::is_volatile_v<std::remove_reference_t<T>>) cv += " volatile";
    if (cv.length() > 0) cv = "[" + cv + " ]";
    s = s + " " + cv;
    return s;
  }
};
template <typename T>
std::string to_bits(const T& var) {
  // static_assert(std::is_trivially_copyable_v<T>,
  //               "T must be trivially copyable.");
  constexpr size_t num_bits = sizeof(T) * 8;
  const volatile unsigned char* bytes =
      reinterpret_cast<const volatile unsigned char*>(&var);
  std::ostringstream oss;
  for (int i = 0; i < sizeof(T); ++i) {
    for (int j = 7; j >= 0; --j) {
      char b = *bytes;
      oss << ((b >> j) & 1);
    }
    oss << ' ';
    bytes++;
  }
  return oss.str();
}
// sizeofable traits
template <typename T, typename = void>
struct is_sizeofable : std::false_type {};
template <typename T>
struct is_sizeofable<T, std::void_t<decltype(sizeof(T))>> : std::true_type {};
// typeidable traits
template <typename T, typename = void>
struct is_typeidable : std::false_type {};
template <typename T>
struct is_typeidable<T, std::void_t<decltype(typeid(T).name())>>
    : std::true_type {};
// void* traits
template <typename T>
struct is_void_pointer : std::false_type {};
template <>
struct is_void_pointer<void*> : std::true_type {};
// output traits
template <typename T, typename = void>
struct is_outputable : std::false_type {};
template <typename T>
struct is_outputable<T, std::void_t<decltype(std::declval<std::ostream&>()
                                             << std::declval<T>())>>
    : std::true_type {};
// de-reference traits
template <typename T, typename = void>
struct is_dereferenceable : std::false_type {};
template <typename T>
struct is_dereferenceable<T, std::void_t<decltype(*std::declval<T>())>>
    : std::true_type {};
// pointer depth
template <typename T>
struct ptr_depth_impl {
  static constexpr size_t value = 0;
};
template <typename T>
struct ptr_depth_impl<T*> {
  static constexpr size_t value =
      1 + ptr_depth_impl<std::remove_cv_t<std::remove_reference_t<T>>>::value;
};
template <typename T>
struct pointer_depth {
  static constexpr size_t value =
      ptr_depth_impl<std::remove_cv_t<std::remove_reference_t<T>>>::value;
};
template <typename T, typename G>
void analyzeType_impl(G&& var) {
  using CURRENT_TYPE = typename std::conditional_t<std::is_reference_v<G>, T, T&&>;
  using DEREFED_CURRENT_TYPE = std::remove_reference_t<CURRENT_TYPE>;
  using Pointee = std::remove_pointer_t<DEREFED_CURRENT_TYPE>;
  size_t level = pointer_depth<DEREFED_CURRENT_TYPE>::value;
  std::cout << "-------------层级: " << level << " ----------------"
            << std::endl;
  std::string type_name = my_demangle<CURRENT_TYPE>::name();
  std::cout << " 数据类型: " << type_name << std::endl;
  std::cout << " 数据地址: " << (void*)&var << std::endl;
  if constexpr (std::is_pointer_v<DEREFED_CURRENT_TYPE>) {
    std::cout << " 字节数: " << sizeof(DEREFED_CURRENT_TYPE) << std::endl;
    ptr_addr addr = reinterpret_cast<ptr_addr>(var);
    std::cout << " 指向的地址 (hex): 0x" << std::hex << addr << std::dec
              << std::endl;
    if constexpr (!is_typeidable<Pointee>::value) {
      std::cout
          << " 这个指针指向一个非标准的非完整类型(non-standard incomplete "
             "type)，到此为止。"
          << std::endl;
    } else {
      std::cout << " 指向的数据类型: " << my_demangle<Pointee>::name()
                << std::endl;
      if constexpr (!is_dereferenceable<DEREFED_CURRENT_TYPE>::value) {
        std::cout << " 此类型的指针无法解引用" << std::endl;
      } else {
        analyzeType_impl<Pointee>(*var);
      }
    }
  } else {
    if constexpr (!is_sizeofable<DEREFED_CURRENT_TYPE>::value) {
      std::cout << " 此类型没有size" << std::endl;
      if constexpr (is_outputable<DEREFED_CURRENT_TYPE>::value) {
        std::cout << " 打印值: " << var << std::endl;
      } else {
        std::cout << " 此类型不可打印" << std::endl;
      }
    } else {
      std::cout << " 字节数: " << sizeof(DEREFED_CURRENT_TYPE) << std::endl;
      if (!is_pointer_safe_to_read(reinterpret_cast<ptr_addr>(&var))) {
        std::cout << " 此区域不可访问" << std::endl;
      } else {
        std::cout << " 内存中的二进制表示（低地址到高地址）： " << to_bits(var)
                  << std::endl;
        if constexpr (is_outputable<DEREFED_CURRENT_TYPE>::value) {
          std::cout << " 打印值: " << var << std::endl;
        } else {
          std::cout << " 此类型不可打印" << std::endl;
        }
      }
    }
  }
}
