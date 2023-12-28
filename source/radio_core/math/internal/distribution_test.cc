// Copyright (c) 2021 radio core authors
//
// SPDX-License-Identifier: MIT

#include "radio_core/math/distribution.h"

#include <array>
#include <random>

#include "radio_core/unittest/test.h"

namespace radio_core {

////////////////////////////////////////////////////////////////////////////////
// Uniform distribution.

// Validate that the entire [a .. b] segment is covered.
TEST(UniformRealDistribution, Range) {
  class FixedGenerator {
   public:
    explicit FixedGenerator(int value) : value_(value) {}

    inline auto operator()() const -> int { return value_; }

    // NOLINTBEGIN(readability-identifier-naming)
    inline auto min() const -> int { return -32767; }
    inline auto max() const -> int { return 32767; }
    // NOLINTEND(readability-identifier-naming)

   private:
    int value_;
  };

  {
    FixedGenerator generator(32767);
    UniformRealDistribution distribution(2.0f, 12.0f);
    EXPECT_NEAR(distribution(generator), 12.0f, 1e-6f);
  }

  {
    FixedGenerator generator(-32767);
    UniformRealDistribution distribution(2.0f, 12.0f);
    EXPECT_NEAR(distribution(generator), 2.0f, 1e-6f);
  }
}

// Validate that the distribution gives matched results on all platforms.
TEST(UniformRealDistribution, Distribution) {
  constexpr auto kExpectedValues = std::to_array({
      10.1472368240f, 3.3547701836f,  11.0579195023f, 10.3500852585f,
      3.2698681355f,  11.6886777878f, 11.1337585449f, 4.2103404999f,
      8.3235931396f,  5.0816702843f,  2.9754040241f,  7.4722061157f,
      4.7849822044f,  3.8838195801f,  7.4688148499f,  11.9288129807f,
      11.5750684738f, 11.9646129608f, 11.6488847733f, 11.6769495010f,
      3.5761308670f,  9.2583894730f,  11.7059278488f, 11.8110971451f,
      11.5716695786f, 3.0986175537f,  6.8537564278f,  9.9810581207f,
      10.0028047562f, 4.9702944756f,  3.4188632965f,  2.0478348732f,
      6.2176127434f,  3.1246452332f,  11.1573553085f, 8.3976335526f,
      9.9220733643f,  10.7843065262f, 11.5949249268f, 7.0366268158f,
      8.5574073792f,  9.9792861938f,  2.3571166992f,  5.6129398346f,
      10.4912929535f, 4.1192436218f,  11.3399324417f, 8.8135948181f,
      8.7873516083f,  5.9873852730f,  9.5774011612f,  9.4064731598f,
      9.4313249588f,  6.7475867271f,  5.9222702980f,  6.2208766937f,
      8.5547790527f,  3.7386517525f,  3.7118668556f,  5.0191311836f,
      9.0604610443f,  9.9727993011f,  2.3183283806f,  5.1655044556f,
      4.7692298889f,  10.7242879868f, 2.4617137909f,  3.4911398888f,
      2.9713177681f,  11.9406852722f, 10.2345781326f, 10.2190332413f,
      8.9482860565f,  3.2518277168f,  5.1709947586f,  9.6375007629f,
      11.5022211075f, 6.9058904648f,  2.3444607258f,  8.6360549927f,
      6.3874435425f,  3.2589664459f,  5.8155841827f,  4.1020908356f,
      9.6551685333f,  2.5121643543f,  9.9519996643f,  2.3644125462f,
      3.8687260151f,  6.0873117447f,  6.8976440430f,  6.5798916817f,
      6.4558620453f,  6.8756890297f,  8.4631299973f,  9.9397497177f,
      9.0936489105f,  11.2087478638f, 9.5468664169f,  10.0753097534f,
  });

  std::mt19937 generator(5489);
  UniformRealDistribution distribution(2.0f, 12.0f);
  for (int i = 0; i < 100; ++i) {
    const float actual_value = distribution(generator);
    EXPECT_NEAR(actual_value, kExpectedValues[i], 1e-6f)
        << "Mismatch at index " << i;
  }
}

////////////////////////////////////////////////////////////////////////////////
// normal distribution.

TEST(NormalDistribution, Distribution) {
  constexpr auto kExpectedValues = std::to_array({
      0.253161013126f,  -0.293219149113f, 0.084590151906f,  -0.057085510343f,
      0.992327749729f,  -1.438215613365f, -0.910654842854f, 0.106847144663f,
      -0.600247025490f, -0.844452857971f, 0.018782431260f,  0.197466164827f,
      -0.743001043797f, 0.490084707737f,  -0.070391684771f, 1.434878230095f,
      0.939871490002f,  -0.635293602943f, -0.191795349121f, -0.950009107590f,
      0.686196446419f,  0.230687797070f,  0.258587568998f,  0.334890574217f,
      0.581200480461f,  0.004632864147f,  0.583762109280f,  1.116724610329f,
      -0.339338600636f, -0.101377300918f, 0.486659288406f,  -0.401555240154f,
      0.456154614687f,  0.190620452166f,  1.641196489334f,  -0.929811418056f,
      0.863877058029f,  0.806586205959f,  1.682867288589f,  -0.174710452557f,
      -1.865247607231f, -1.348443627357f, 0.490576535463f,  -1.029047250748f,
      -0.880735158920f, -0.530580937862f, 0.648283600807f,  0.935332834721f,
      -0.386271506548f, 0.644883275032f,  0.428506463766f,  0.426447033882f,
      0.378740042448f,  -0.728631615639f, -0.758809387684f, 1.094234108925f,
      0.646847724915f,  -0.013520994224f, -0.216434076428f, 0.076059520245f,
      -0.170043200254f, -1.038496017456f, -0.517756462097f, -1.266794681549f,
      -1.255027294159f, -0.365809202194f, -0.740716993809f, -3.040185451508f,
      -2.887072086334f, -0.659565091133f, 0.577839076519f,  1.161005735397f,
      0.221297532320f,  0.444862455130f,  0.604959547520f,  0.730481207371f,
      -1.038377046585f, 0.953996300697f,  0.954159259796f,  1.296244978905f,
      -1.023785352707f, 0.436841249466f,  -1.023138403893f, -0.118068888783f,
      -0.009265811183f, 1.551442146301f,  0.527050912380f,  0.084552422166f,
      -0.349315583706f, 0.824533104897f,  0.356598287821f,  1.288821339607f,
      0.556164979935f,  0.711059212685f,  -0.561515629292f, 0.735620737076f,
      0.009668905288f,  0.714283406734f,  1.671511292458f,  -0.733279526234f,
  });

  std::mt19937 generator(5489);
  NormalDistribution distribution(0.0f, 1.0f);
  for (int i = 0; i < 100; ++i) {
    const float actual_value = distribution(generator);
    // NOTE: Lower than one would expect epsilon to make it possible to use
    // -ffast-math globally in the project. Since the normal distribution uses
    // trigonometric functions this flag affects accuracy of calculation.
    EXPECT_NEAR(actual_value, kExpectedValues[i], 1e-6f)
        << "Mismatch at index " << i;
  }
}

}  // namespace radio_core
