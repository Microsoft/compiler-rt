// Check that without suppressions, we catch the issue.
// RUN: %clangxx_asan -O0 %s -o %t -framework Foundation
// RUN: not %run %t 2>&1 | FileCheck --check-prefix=CHECK-CRASH %s

// Check that suppressing a function name works within a no-fork sandbox
// RUN: echo "interceptor_via_fun:CFStringCreateWithBytes" > %t.supp
// RUN: env ASAN_OPTIONS=$ASAN_OPTIONS:suppressions=%t.supp \
// RUN:   sandbox-exec -p '(version 1)(allow default)(deny process-fork)' \
// RUN:   %run %t 2>&1 | FileCheck --check-prefix=CHECK-IGNORE %s

#include <CoreFoundation/CoreFoundation.h>

int main() {
  char *a = (char *)malloc(6);
  strcpy(a, "hello");
  CFStringRef str =
      CFStringCreateWithBytes(kCFAllocatorDefault, (unsigned char *)a, 10,
                              kCFStringEncodingUTF8, FALSE);  // BOOM
  fprintf(stderr, "Ignored.\n");
  free(a);
}

// CHECK-CRASH: AddressSanitizer: heap-buffer-overflow
// CHECK-CRASH-NOT: Ignored.
// CHECK-IGNORE-NOT: AddressSanitizer: heap-buffer-overflow
// CHECK-IGNORE: Ignored.
