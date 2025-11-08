#include <gtest/gtest.h>

#include "AES/AES11/2009/core/sample_rate_validation.hpp"

#ifdef AES5_INTEGRATION
#include "AES/AES5/2018/rates/standard_sampling_rates.hpp"
using AES::AES5::_2018::rates::StandardSamplingRates;
#endif

using AES::AES11::_2009::core::SampleRateValidator;

// REQ-F-DARS-008: Validate sampling frequency against AES5; reject non-standard; check ppm tolerance and structured evaluation

TEST(SampleRateValidationTests, AcceptsAES5StandardRates) {
#ifdef AES5_INTEGRATION
    EXPECT_TRUE(SampleRateValidator::is_aes5_standard(StandardSamplingRates::RATE_32000_HZ));
    EXPECT_TRUE(SampleRateValidator::is_aes5_standard(StandardSamplingRates::RATE_44100_HZ));
    EXPECT_TRUE(SampleRateValidator::is_aes5_standard(StandardSamplingRates::RATE_48000_HZ));
    EXPECT_TRUE(SampleRateValidator::is_aes5_standard(StandardSamplingRates::RATE_96000_HZ));
    EXPECT_TRUE(SampleRateValidator::is_aes5_standard(StandardSamplingRates::RATE_192000_HZ));
#else
    EXPECT_TRUE(SampleRateValidator::is_aes5_standard(32000));
    EXPECT_TRUE(SampleRateValidator::is_aes5_standard(44100));
    EXPECT_TRUE(SampleRateValidator::is_aes5_standard(48000));
    EXPECT_TRUE(SampleRateValidator::is_aes5_standard(96000));
    EXPECT_TRUE(SampleRateValidator::is_aes5_standard(192000));
#endif
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
