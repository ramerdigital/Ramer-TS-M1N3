#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>

int main()
{
    std::ifstream jsonFile("models/model_ts9_48k_cond2.json");
    if (!jsonFile.is_open())
    {
        std::cerr << "Error: Could not open models/model_ts9_48k_cond2.json\n";
        return 1;
    }

    nlohmann::json j;
    try
    {
        jsonFile >> j;
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error parsing JSON: " << e.what() << "\n";
        return 1;
    }

    std::vector<uint8_t> msgpackData = nlohmann::json::to_msgpack(j);

    std::ofstream msgpackFile("models/model_ts9_48k_cond2.msgpack", std::ios::binary);
    if (!msgpackFile.is_open())
    {
        std::cerr << "Error: Could not open models/model_ts9_48k_cond2.msgpack for writing\n";
        return 1;
    }

    msgpackFile.write(reinterpret_cast<const char*>(msgpackData.data()), msgpackData.size());
    std::cout << "Successfully serialized JSON to MessagePack format!\n";
    std::cout << "Compressed MessagePack size: " << msgpackData.size() << " bytes\n";

    return 0;
}
