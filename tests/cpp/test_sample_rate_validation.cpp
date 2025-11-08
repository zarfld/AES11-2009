#include <gtest/gtest.h>

#include "AES/AES11/2009/core/sample_rate_validation.hpp"


using AES::AES11::_2009::core::SampleRateValidator;

// REQ-F-DARS-008: Validate sampling frequency against AES5; reject non-standard; check ppm tolerance and structured evaluation

TEST(SampleRateValidationTests, AcceptsAES5StandardRates) {
    EXPECT_TRUE(SampleRateValidator::is_aes5_standard(32000));
    EXPECT_TRUE(SampleRateValidator::is_aes5_standard(44100));
    EXPECT_TRUE(SampleRateValidator::is_aes5_standard(48000));
    EXPECT_TRUE(SampleRateValidator::is_aes5_standard(96000));
    EXPECT_TRUE(SampleRateValidator::is_aes5_standard(192000));
}

TEST(SampleRateValidationTests, RejectsNonStandardRate) {
    EXPECT_FALSE(SampleRateValidator::is_aes5_standard(47999));
    EXPECT_FALSE(SampleRateValidator::is_aes5_standard(12345));
}

TEST(SampleRateValidationTests, StructuredEvaluationProvidesPpmError) {
    auto result = SampleRateValidator::evaluate(48000, 48000.0);
    EXPECT_TRUE(result.isStandard);
    EXPECT_NEAR(result.ppmError, 0.0, 1e-9);
    double plus8ppm = 48000.0 * (1.0 + 8.0 / 1'000'000.0);
    auto result2 = SampleRateValidator::evaluate(48000, plus8ppm);
    EXPECT_NEAR(result2.ppmError, 8.0, 0.05);
}

TEST(SampleRateValidationTests, PpmToleranceValidation) {
    const uint32_t nominal = 48000;
    EXPECT_TRUE(SampleRateValidator::within_tolerance(nominal, 48000.0, 1.0));
    double plus5ppm = 48000.0 * (1.0 + 5.0 / 1'000'000.0);
    EXPECT_TRUE(SampleRateValidator::within_tolerance(nominal, plus5ppm, 10.0));
    double plus12ppm = 48000.0 * (1.0 + 12.0 / 1'000'000.0);
    EXPECT_FALSE(SampleRateValidator::within_tolerance(nominal, plus12ppm, 10.0));
    EXPECT_FALSE(SampleRateValidator::within_tolerance(nominal, 48000.0, -1.0));
}
