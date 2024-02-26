#include <check_functions.h>
#include <gtest/gtest.h>

TEST(check_functions, ip_check) {
  EXPECT_TRUE(ip_check("255.255.255.255"));
  EXPECT_FALSE(ip_check("255.255.255"));
  EXPECT_FALSE(ip_check("255525"));
  EXPECT_FALSE(ip_check("-5525"));
  EXPECT_FALSE(ip_check("25552a5"));
  EXPECT_TRUE(ip_check("0.0.0.0"));
  EXPECT_TRUE(ip_check("127.0.0.1"));
  EXPECT_FALSE(ip_check(""));
}

TEST(check_functions, port_check) {
  EXPECT_TRUE(port_check("0", "0"));
  EXPECT_TRUE(port_check("1250", "65535"));
  EXPECT_TRUE(port_check("65535", "65535"));
  EXPECT_TRUE(port_check("9000", "10000"));

  EXPECT_FALSE(port_check("", "5"));
  EXPECT_FALSE(port_check("-1", "0"));
  EXPECT_FALSE(port_check("65536", "65537"));
  EXPECT_FALSE(port_check("5a36", "6877"));
  EXPECT_FALSE(port_check("999999", "9999999"));
  EXPECT_FALSE(port_check("1000", "1"));
}

TEST(check_functions, argument_check) {
  int argc = 4;
  char const *argv[argc];
  argv[0] = "file_name";
  argv[1] = "255.255.255.255";
  argv[2] = "125";
  argv[3] = "200";
  EXPECT_TRUE(argument_check(4, argv));

  EXPECT_FALSE(argument_check(3, argv));
}