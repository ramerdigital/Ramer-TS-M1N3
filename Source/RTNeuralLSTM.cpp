#include "RTNeuralLSTM.h"
#include <iostream>

using Vec2d = std::vector<std::vector<float>>;

Vec2d transpose(const Vec2d& x)
{
    if (x.empty() || x[0].empty())
        return {};

    auto outer_size = x.size();
    auto inner_size = x[0].size();
    Vec2d y(inner_size, std::vector<float>(outer_size, 0.0f));

    for (size_t i = 0; i < outer_size; ++i)
    {
        for (size_t j = 0; j < inner_size; ++j)
            y[j][i] = x[i][j];
    }

    return y;
}


bool RT_LSTM::load_json3(const TS_JSON& weights_json)
{
    try
    {
        auto& lstm = model_cond2.get<0>();
        auto& dense = model_cond2.get<1>();

        auto ih_ptr = "/state_dict/rec.weight_ih_l0"_json_pointer;
        auto hh_ptr = "/state_dict/rec.weight_hh_l0"_json_pointer;
        auto bias_ih_ptr = "/state_dict/rec.bias_ih_l0"_json_pointer;
        auto bias_hh_ptr = "/state_dict/rec.bias_hh_l0"_json_pointer;
        auto dense_w_ptr = "/state_dict/lin.weight"_json_pointer;
        auto dense_b_ptr = "/state_dict/lin.bias"_json_pointer;

        if (!weights_json.contains(ih_ptr) || !weights_json.contains(hh_ptr) ||
            !weights_json.contains(bias_ih_ptr) || !weights_json.contains(bias_hh_ptr) ||
            !weights_json.contains(dense_w_ptr) || !weights_json.contains(dense_b_ptr))
        {
            std::cerr << "Error: Missing required keys in model weights.\n";
            return false;
        }

        const auto& json_ih = weights_json[ih_ptr];
        if (!json_ih.is_array()) return false;
        Vec2d lstm_weights_ih = json_ih;
        if (lstm_weights_ih.size() != 128 || lstm_weights_ih[0].size() != 3)
        {
            std::cerr << "Error: Invalid shape for lstm_weights_ih.\n";
            return false;
        }
        lstm.setWVals(transpose(lstm_weights_ih));

        const auto& json_hh = weights_json[hh_ptr];
        if (!json_hh.is_array()) return false;
        Vec2d lstm_weights_hh = json_hh;
        if (lstm_weights_hh.size() != 128 || lstm_weights_hh[0].size() != 32)
        {
            std::cerr << "Error: Invalid shape for lstm_weights_hh.\n";
            return false;
        }
        lstm.setUVals(transpose(lstm_weights_hh));

        const auto& json_bias_ih = weights_json[bias_ih_ptr];
        const auto& json_bias_hh = weights_json[bias_hh_ptr];
        if (!json_bias_ih.is_array() || !json_bias_hh.is_array()) return false;
        std::vector<float> lstm_bias_ih = json_bias_ih;
        std::vector<float> lstm_bias_hh = json_bias_hh;
        if (lstm_bias_ih.size() != 128 || lstm_bias_hh.size() != 128)
        {
            std::cerr << "Error: Invalid shape for LSTM biases.\n";
            return false;
        }
        for (int i = 0; i < 128; ++i)
            lstm_bias_hh[i] += lstm_bias_ih[i];
        lstm.setBVals(lstm_bias_hh);

        const auto& json_dense_w = weights_json[dense_w_ptr];
        if (!json_dense_w.is_array()) return false;
        Vec2d dense_weights = json_dense_w;
        if (dense_weights.size() != 1 || dense_weights[0].size() != 32)
        {
            std::cerr << "Error: Invalid shape for dense_weights.\n";
            return false;
        }
        dense.setWeights(dense_weights);

        const auto& json_dense_b = weights_json[dense_b_ptr];
        if (!json_dense_b.is_array()) return false;
        std::vector<float> dense_bias = json_dense_b;
        if (dense_bias.size() != 1)
        {
            std::cerr << "Error: Invalid shape for dense_bias.\n";
            return false;
        }
        dense.setBias(dense_bias.data());
    }
    catch (const nlohmann::json::exception& e)
    {
        std::cerr << "JSON parser exception in load_json3: " << e.what() << "\n";
        return false;
    }
    catch (const std::exception& e)
    {
        std::cerr << "Standard exception in load_json3: " << e.what() << "\n";
        return false;
    }

    return true;
}

void RT_LSTM::reset()
{
    model_cond2.reset();
}

void RT_LSTM::process(const float* inData, float param1, float param2, float* outData, int numSamples)
{
    // Check for parameter changes for smoothing calculations
    if (param1 != previousParam1) {
        steppedValue1 = (param1 - previousParam1) / static_cast<float> (numSamples);
        changedParam1 = true;
    } else {
        changedParam1 = false;
    }
    if (param2 != previousParam2) {
        steppedValue2 = (param2 - previousParam2) / static_cast<float> (numSamples);
        changedParam2 = true;
    } else {
        changedParam2 = false;
    }

    // Loop unswitching to remove conditional branches from the hot loop
    if (!changedParam1 && !changedParam2)
    {
        inArray[1] = param1;
        inArray[2] = param2;
        for (int i = 0; i < numSamples; ++i)
        {
            inArray[0] = inData[i];
            outData[i] = model_cond2.forward(inArray) + inData[i];
        }
    }
    else if (changedParam1 && !changedParam2)
    {
        inArray[2] = param2;
        for (int i = 0; i < numSamples; ++i)
        {
            inArray[0] = inData[i];
            inArray[1] = previousParam1 + static_cast<float> (i + 1) * steppedValue1;
            outData[i] = model_cond2.forward(inArray) + inData[i];
        }
    }
    else if (!changedParam1 && changedParam2)
    {
        inArray[1] = param1;
        for (int i = 0; i < numSamples; ++i)
        {
            inArray[0] = inData[i];
            inArray[2] = previousParam2 + static_cast<float> (i + 1) * steppedValue2;
            outData[i] = model_cond2.forward(inArray) + inData[i];
        }
    }
    else // changedParam1 && changedParam2
    {
        for (int i = 0; i < numSamples; ++i)
        {
            inArray[0] = inData[i];
            inArray[1] = previousParam1 + static_cast<float> (i + 1) * steppedValue1;
            inArray[2] = previousParam2 + static_cast<float> (i + 1) * steppedValue2;
            outData[i] = model_cond2.forward(inArray) + inData[i];
        }
    }

    previousParam1 = param1;
    previousParam2 = param2;
}
