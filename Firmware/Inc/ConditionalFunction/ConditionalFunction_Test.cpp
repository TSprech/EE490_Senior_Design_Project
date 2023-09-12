#include "ConditionalFunction.hpp"

#include "gtest/gtest.h"

auto VoidCallable = [] {};
auto IntCallable = [] { return 123; };

TEST(FunctionManagerTest, Construct) {
  cfunc::FunctionManager void_function(VoidCallable);
  cfunc::FunctionManager int_function(IntCallable);
  EXPECT_TRUE(true) << "FunctionManager constructor has failed";
}

TEST(FunctionManagerTest, EnableDisable) {
  cfunc::FunctionManager funcManager(IntCallable);

  EXPECT_FALSE(funcManager()) << "Function should not be invocable in its initial disabled state";

  funcManager.Enable();
  EXPECT_TRUE(true) << "Function invocation has failed after enabling";

  funcManager.Disable();
  EXPECT_FALSE(funcManager()) << "Function should not be invocable after disabling";
}

TEST(FunctionManagerTest, Invoke) {
  cfunc::FunctionManager funcManager(VoidCallable);
  funcManager.Enable();
  EXPECT_TRUE(true) << "Void function invocation has crashed";

  cfunc::FunctionManager funcManager2(IntCallable);
  funcManager2.Enable();
  auto option = funcManager2();
  ASSERT_TRUE(option.has_value()) << "Function invocation didn't return a value after enabling";
  EXPECT_EQ(option.value(), 123) << "Returned value is not correct";
}

struct CustomStruct {
  int a;
  double b;

  bool operator==(const CustomStruct &other) const {
    return a == other.a && b == other.b;
  }
};

auto ComplexCallable = [] { return CustomStruct{2, 3.5}; };

TEST(FunctionManagerTest, InvokeComplexType) {
  cfunc::FunctionManager funcManager(ComplexCallable);

  auto option = funcManager();
  EXPECT_FALSE(option.has_value()) << "Function invocation should not return a value when disabled";

  funcManager.Enable();
  option = funcManager();
  ASSERT_TRUE(option.has_value()) << "Function invocation didn't return a value after enabling";
  EXPECT_EQ(option.value(), (CustomStruct{2, 3.5})) << "Returned struct is not correct";

  funcManager.Disable();
  option = funcManager();
  ASSERT_FALSE(option.has_value()) << "Function invocation should not return a value when disabled";
}