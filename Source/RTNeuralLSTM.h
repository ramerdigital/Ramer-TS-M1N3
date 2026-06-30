#pragma once

#include <nlohmann/json.hpp>
#include <RTNeural/RTNeural.h>
#include "JSONAllocator.h"

using TS_JSON = nlohmann::basic_json<
    std::map,
    std::vector,
    std::string,
    bool,
    std::int64_t,
    std::uint64_t,
    double,
    MyJSONAllocator
>;

class RT_LSTM
{
public:
    RT_LSTM() = default;

    void reset();
    //void load_json3(const char* jsonFile);
    bool load_json3(const TS_JSON& weights_json);

    //void process(const float* inData, float* outData, int numSamples);
    void process(const float* inData, float param1, float param2, float* outData, int numSamples);

    int input_size = 3;

    float previousParam1 = 0.0;
    float previousParam2 = 0.0;
    float steppedValue1 = 0.0;
    float steppedValue2 = 0.0;
    bool changedParam1 = false;
    bool changedParam2 = false;

private:
    RTNeural::ModelT<float, 3, 1,
        RTNeural::LSTMLayerT<float, 3, 32>,
        RTNeural::DenseT<float, 32, 1>> model_cond2;

    float inArray alignas(16)[4] = { 0.0, 0.0, 0.0, 0.0 };
    //float inArray[2] = { 0.0, 0.0, 0.0 };
};
