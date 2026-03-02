/*
 * Unit tests for Sharky774 wM-Bus driver register parsing.
 *
 * Reference telegram (encrypted, key not included in this test):
 *   5E44A5111828058141046DF17A460050057960085091B6F8361852F7972492760
 *   B6478A83790B67829B74A26C79E00C0F984BB5A139EC13AD86B45CF5E9A9920A
 *   C3494561AC388B650CECF7CBC9143D7D05086B7DB940D6F29D612BD97A37B7F9
 *   A67EC94734EDADAB93D4D832...
 *
 * Expected decoded values:
 *   flow_temperature_c          : 44.6
 *   operating_time_h            : 23662
 *   operating_time_in_error_h   : 0
 *   power_kw                    : 0.8
 *   return_temperature_c        : 31.5
 *   total_energy_consumption_kwh: 5269.722222
 *   total_volume_m3             : 538.96
 *   volume_flow_m3h             : 0.053
 *
 * The test below uses a pre-decoded telegram constructed from those values,
 * encoding each field in the DIF/VIF format that the Sharky774 driver expects.
 */

#include <unity.h>
#include "../../lib/wmbus/src/Drivers/driver_sharky774.h"

// ---------------------------------------------------------------------------
// Build a synthetic (pre-decrypted, CRC-stripped) telegram with all required
// fields embedded as standard wM-Bus variable-data records.
//
// Byte layout:
//   [0..9]   DLL header  (preserved from the reference telegram)
//   [10]     CI = 0x78   (variable-data, no transport-layer header)
//   [11..]   DIF/VIF data records (parsed by the driver)
// ---------------------------------------------------------------------------
static std::vector<unsigned char> build_sharky774_telegram() {
    return {
        // --- DLL header (10 bytes) ---
        0x5E, 0x44, 0xA5, 0x11, 0x18, 0x28, 0x05, 0x41, 0x41, 0x04,
        // CI field (byte 10)
        0x78,

        // --- Data records (driver scans from byte 11) ---

        // total_energy_consumption_kwh = 5269.722222 kWh
        //   DIF=0x0C (4-digit BCD), VIF=0x0E (energy in MJ)
        //   stored value: 18971 (MJ), driver divides by 3.6 → 5269.7222 kWh
        //   BCD4 little-endian: 18971 → 0x71, 0x89, 0x01, 0x00
        0x0C, 0x0E,  0x71, 0x89, 0x01, 0x00,

        // total_volume_m3 = 538.96 m3
        //   DIF=0x0C (4-digit BCD), VIF=0x13 (volume in l)
        //   stored value: 538960 (l), driver divides by 1000 → 538.96 m3
        //   BCD4 little-endian: 538960 → 0x60, 0x89, 0x53, 0x00
        0x0C, 0x13,  0x60, 0x89, 0x53, 0x00,

        // power_kw = 0.8 kW
        //   DIF=0x0A (2-digit BCD), VIF=0x2D (power in W × 10^-1)
        //   stored value: 8, driver divides by 10 → 0.8 kW
        //   BCD2 little-endian: 8 → 0x08, 0x00
        0x0A, 0x2D,  0x08, 0x00,

        // flow_temperature_c = 44.6 °C
        //   DIF=0x0A (2-digit BCD), VIF=0x5A (flow temp in °C × 10^-1)
        //   stored value: 446, driver divides by 10 → 44.6 °C
        //   BCD2 little-endian: 446 → 0x46, 0x04
        0x0A, 0x5A,  0x46, 0x04,

        // return_temperature_c = 31.5 °C
        //   DIF=0x0A (2-digit BCD), VIF=0x5E (return temp in °C × 10^-1)
        //   stored value: 315, driver divides by 10 → 31.5 °C
        //   BCD2 little-endian: 315 → 0x15, 0x03
        0x0A, 0x5E,  0x15, 0x03,

        // operating_time_h = 23662 h
        //   DIF=0x04 (32-bit integer LE), VIF=0x76 (operating time in hours)
        //   23662 = 0x00005C6E → LE bytes: 0x6E, 0x5C, 0x00, 0x00
        0x04, 0x76,  0x6E, 0x5C, 0x00, 0x00,

        // operating_time_in_error_h = 0 h
        //   DIF=0x04 (32-bit integer LE), VIF=0x7E (time in error state, hours)
        //   0 → 0x00, 0x00, 0x00, 0x00
        0x04, 0x7E,  0x00, 0x00, 0x00, 0x00,

        // volume_flow_m3h = 0.053 m3/h
        //   DIF=0x0A (2-digit BCD), VIF=0x3B (volume flow in l/h)
        //   stored value: 53 (l/h), driver divides by 1000 → 0.053 m3/h
        //   BCD2 little-endian: 53 → 0x53, 0x00
        0x0A, 0x3B,  0x53, 0x00,

        // padding (keeps the scanner from reading past the vector boundary)
        0x00, 0x00, 0x00, 0x00
    };
}

// ---------------------------------------------------------------------------
// Test fixtures
// ---------------------------------------------------------------------------
static Sharky774 driver;
static std::map<std::string, float> values;

void setUp() {
    auto telegram = build_sharky774_telegram();
    values = driver.get_values(telegram);
}

void tearDown() {
    values.clear();
}

// ---------------------------------------------------------------------------
// Individual field tests
// ---------------------------------------------------------------------------
void test_driver_name() {
    TEST_ASSERT_EQUAL_STRING("sharky774", driver.get_name().c_str());
}

void test_returns_non_empty_map() {
    TEST_ASSERT_TRUE(values.size() > 0);
}

void test_total_energy_consumption_kwh() {
    TEST_ASSERT_TRUE(values.count("total_energy_consumption_kwh") > 0);
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 5269.722f, values.at("total_energy_consumption_kwh"));
}

void test_total_volume_m3() {
    TEST_ASSERT_TRUE(values.count("total_volume_m3") > 0);
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 538.96f, values.at("total_volume_m3"));
}

void test_power_kw() {
    TEST_ASSERT_TRUE(values.count("power_kw") > 0);
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 0.8f, values.at("power_kw"));
}

void test_flow_temperature_c() {
    TEST_ASSERT_TRUE(values.count("flow_temperature_c") > 0);
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 44.6f, values.at("flow_temperature_c"));
}

void test_return_temperature_c() {
    TEST_ASSERT_TRUE(values.count("return_temperature_c") > 0);
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 31.5f, values.at("return_temperature_c"));
}

void test_operating_time_h() {
    TEST_ASSERT_TRUE(values.count("operating_time_h") > 0);
    TEST_ASSERT_FLOAT_WITHIN(0.5f, 23662.0f, values.at("operating_time_h"));
}

void test_operating_time_in_error_h() {
    TEST_ASSERT_TRUE(values.count("operating_time_in_error_h") > 0);
    TEST_ASSERT_FLOAT_WITHIN(0.5f, 0.0f, values.at("operating_time_in_error_h"));
}

void test_volume_flow_m3h() {
    TEST_ASSERT_TRUE(values.count("volume_flow_m3h") > 0);
    TEST_ASSERT_FLOAT_WITHIN(0.0001f, 0.053f, values.at("volume_flow_m3h"));
}

// ---------------------------------------------------------------------------
// Test runner
// ---------------------------------------------------------------------------
#ifdef ARDUINO
#include <Arduino.h>
void setup() {
    Serial.begin(115200);
    delay(2000);

    UNITY_BEGIN();

    RUN_TEST(test_driver_name);
    RUN_TEST(test_returns_non_empty_map);
    RUN_TEST(test_total_energy_consumption_kwh);
    RUN_TEST(test_total_volume_m3);
    RUN_TEST(test_power_kw);
    RUN_TEST(test_flow_temperature_c);
    RUN_TEST(test_return_temperature_c);
    RUN_TEST(test_operating_time_h);
    RUN_TEST(test_operating_time_in_error_h);
    RUN_TEST(test_volume_flow_m3h);

    UNITY_END();
}
void loop() {}
#else
int main(int /*argc*/, char ** /*argv*/) {
    UNITY_BEGIN();

    RUN_TEST(test_driver_name);
    RUN_TEST(test_returns_non_empty_map);
    RUN_TEST(test_total_energy_consumption_kwh);
    RUN_TEST(test_total_volume_m3);
    RUN_TEST(test_power_kw);
    RUN_TEST(test_flow_temperature_c);
    RUN_TEST(test_return_temperature_c);
    RUN_TEST(test_operating_time_h);
    RUN_TEST(test_operating_time_in_error_h);
    RUN_TEST(test_volume_flow_m3h);

    return UNITY_END();
}
#endif
