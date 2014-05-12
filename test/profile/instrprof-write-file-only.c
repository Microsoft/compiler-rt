// RUN: %clang_profgen -o %t -O3 %s
// RUN: env LLVM_PROFILE_FILE=%t.profraw %run %t
// RUN: llvm-profdata merge -o %t.profdata %t.profraw
// RUN: %clang_profuse=%t.profdata -o - -S -emit-llvm %s | FileCheck %s

int __llvm_profile_runtime = 0;
int __llvm_profile_write_file(void);
void __llvm_profile_set_filename(const char *);
int foo(int);
int main(int argc, const char *argv[]) {
  // CHECK-LABEL: define i32 @main
  // CHECK: br i1 %{{.*}}, label %{{.*}}, label %{{.*}}, !prof !1
  if (argc > 1)
    return 1;

  // Write out the profile.
  __llvm_profile_write_file();

  // Change the profile.
  return foo(0);
}
int foo(int X) {
  // There should be no profiling information for @foo, since it was called
  // after the profile was written (and the atexit was supressed by defining
  // profile_runtime).
  // CHECK-LABEL: define i32 @foo
  // CHECK: br i1 %{{.*}}, label %{{.*}}, label %{{[^,]+$}}
  return X <= 0 ? -X : X;
}
// CHECK: !1 = metadata !{metadata !"branch_weights", i32 1, i32 2}