#pragma once

[[nodiscard]] int run_integral_tests();

extern bool g_test_signal_ready;

[[nodiscard]] bool test_log_contains(const char *marker);

// Drop city/editor session + scenario events and return to main menu so the next
// test's test_ensure_city_session() reloads a fresh map.
void test_reset_session_between_tests();
