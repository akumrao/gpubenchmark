/*
 * Copyright (c) 2024-25 akumrao@yahoo.com
 *
 * FileName:      threadload.h
 * Description:   Json config file

 * Author:        Arvind Umrao <aumrao@google.com>
 *                Rajanee Kumbhar <rajaneek@google.com>
 */


#include "json.h"
#include "json.hpp"
#include <cstdint>
#include <fstream>
#include <ctime>
#include <fstream>
#include <iostream>
#include <sstream>

using namespace std; 

using namespace nlohmann;
using value = nlohmann::json;

#if 1

/**
 * Function:        bool loadFile(const std::string& path, json & root)
 * Description:     Loads a JSON file from the specified file path and parses its contents
 *                  into a JSON object. If the file cannot be opened, the function returns false.
 *                  This function is useful for reading configuration or data files in JSON format.
 * Parameters:      path: const std::string& - The file path of the JSON file to be loaded.
 *                  root: json& - Reference to a JSON object where the parsed data will be stored.
 * Returns:         bool - Returns true if the file is successfully opened and parsed;
 *                         otherwise, returns false.
 */

bool loadFile(const std::string& path, json & root) {
    std::ifstream ifs;
    ifs.open(path.c_str(), std::ifstream::in);
    if (!ifs.is_open())
        return false;
/* thorws std::invalid_argument */
    root = json::parse(ifs); 

    return true;
}

/**
 * Function:        inline bool saveFile(const std::string& path, const json& root, int indent = 4)
 * Description:     Saves a JSON object to a file at the specified path. The JSON data is
 *                  written in a human-readable format with indentation by default. If the
 *                  file cannot be opened for writing, the function returns false.
 * Parameters:      path: const std::string& - The file path where the JSON data will be saved.
 *                  root: const json& - The JSON object containing the data to be written.
 *                  indent: int (default = 4) - The number of spaces used for indentation
 *                         in the JSON file. If set to 0, the JSON is written in a compact format.
 * Returns:         bool - Returns true if the file is successfully opened and written;
 *                         otherwise, returns false.
 */

inline bool saveFile(const std::string& path, const json& root, int indent = 4) {
    std::ofstream ofs(path, std::ios::binary | std::ios::out);
    if (!ofs.is_open())
        return false;

    if (indent > 0)
        ofs << root.dump(indent);
    else
        ofs << root.dump();
    ofs.close();
    return true;
}

/**
 * Function:        inline void stringify(const json& root, std::string& output, bool pretty = false)
 * Description:     Converts a JSON object into a string representation. If the `pretty` flag
 *                  is set to true, the output is formatted with indentation for readability;
 *                  otherwise, a compact string is generated.
 * Parameters:      root: const json& - The JSON object to be converted into a string.
 *                  output: std::string& - Reference to the string where the JSON data will be stored.
 *                  pretty: bool (default = false) - If true, formats the JSON output with indentation.
 * Returns:         void - The resulting JSON string is stored in the `output` parameter.
 */

inline void stringify(const json& root, std::string& output, bool pretty = false) {
    if (pretty) {
        output = root.dump(4);
    } else {
        output = root.dump();
    }
}

/**
 * Function:        inline std::string stringify(const json& root, bool pretty = false)
 * Description:     Converts a JSON object into a string representation and returns it.
 *                  If the `pretty` flag is set to true, the output is formatted with indentation
 *                  for readability; otherwise, a compact string is generated.
 * Parameters:      root: const json& - The JSON object to be converted into a string.
 *                  pretty: bool (default = false) - If true, formats the JSON output with indentation.
 * Returns:         std::string - A string containing the JSON representation of the input object.
 */

inline std::string stringify(const json& root, bool pretty = false) {
    std::string output;
    stringify(root, output, pretty);
    return output;
}



/**
 * Function:        static void saveback(std::vector<std::string>& benchmarks, json & m,
 *                  std::string name, int duration, std::string condition)
 * Description:     Stores benchmarking information into a JSON object. It formats the benchmark
 *                  data as a string and appends it to the "gpu" field in the provided JSON object.
 * Parameters:      benchmarks: std::vector<std::string>& - A reference to a list of benchmark records.
 *                  m: json& - Reference to a JSON object where benchmark data is stored.
 *                  name: std::string - The name of the benchmark test.
 *                  duration: int - The duration of the benchmark test in seconds.
 *                  condition: std::string - Additional conditions or parameters related to the benchmark.
 * Returns:         void - This function does not return a value.
 */

static void saveback(std::vector<std::string>& benchmarks, json & m , std::string name, int duration,  std::string condition ) {
    std::string value = name + ":duration=" + to_string(duration) + ":" + condition;
     
    m["gpu"].push_back(value) ;
}

/**
 * Function:        void initJsonConfig(const char *modeName, int stress_time, char *tmptNode)
 * Description:     Initializes a JSON configuration file for GPU stress testing. The function
 *                  creates a structured JSON object containing various test scenarios such as
 *                  tiles, throttle, stress, power, shader, and blank. It calculates test durations
 *                  dynamically based on the provided stress time and stores the configuration
 *                  in a predefined file path.
 * Parameters:      modeName: const char* - The name of the selected test mode.
 *                  stress_time: int - The total duration allocated for the stress tests.
 *                  tmptNode: char* - Path to the temperature node for monitoring.
 * Returns:         void - This function does not return a value.
 */

void initJsonConfig(const char *modeName, int stress_time, int log_interval, char *tmptNode) {
    json root;
    #if defined(__x86_64__)
      std::string file = "/tmp/config.json";  
    #else
      std::string file = "/storage/emulated/0/Android/data/org.gpu.glload.debug/files/lists/config.json";   
    #endif

    std::vector<std::string> benchmarks;
    {

       root["selected"] = "stress";
       root["duration"] = stress_time;
       root["interval"] = log_interval;
       root["temptNode"] = tmptNode;
       {
            json jtiles;
            jtiles["gpu"] = json::array();
            int countofTest = 8;
            int duration = stress_time/(countofTest+4);
            int left =  stress_time - duration*countofTest;
            saveback(benchmarks, jtiles, "jellyfish", duration, "validate=true");
            saveback(benchmarks, jtiles, "ideas", duration, "validate=true");
            saveback(benchmarks, jtiles, "shading", duration, "validate=true");
            saveback(benchmarks, jtiles ,"bump", duration, "validate=true");
            saveback(benchmarks, jtiles, "shadow", duration, "validate=true");
            saveback(benchmarks, jtiles,"build", duration, "validate=true");
            saveback(benchmarks, jtiles,"terrain", duration, "validate=true");
            saveback(benchmarks, jtiles,"conditionals", duration, "validate=true");
            saveback(benchmarks, jtiles, "buffer", left, "validate=true");
           root["test"]["tiles"] = jtiles;
        }
        {
            int countofTest = 8;
            int duration = stress_time/(countofTest);
            
            json throttle;
            throttle["throttle"] = json::array();

            saveback(benchmarks, throttle, "refract", duration, "show-stats=false");
            saveback(benchmarks, throttle, "jellyfish", duration, "show-stats=false");
            saveback(benchmarks, throttle, "build", duration, "show-stats=false");
            saveback(benchmarks, throttle, "buffer", duration, "show-stats=false");
            saveback(benchmarks, throttle, "refract", duration, "show-stats=false");
            saveback(benchmarks, throttle, "jellyfish", duration, "show-stats=false");
            saveback(benchmarks, throttle, "build", duration, "show-stats=false");
            saveback(benchmarks, throttle, "buffer", duration, "show-stats=false");
           root["test"]["throttle"] = throttle;
        }
        {

            int countofTest = 5;
            int duration = stress_time/(countofTest);

            json jstress;
            jstress["gpu"] = json::array();
            saveback(benchmarks, jstress, "build", duration, "show-stats=false");
            saveback(benchmarks, jstress, "refract", duration, "show-stats=false");
            saveback(benchmarks, jstress, "jellyfish", duration, "show-stats=false");
            saveback(benchmarks, jstress, "refract", duration, "show-stats=false");
            saveback(benchmarks, jstress, "jellyfish", duration, "show-stats=false");


           root["test"]["stress"] = jstress;
        }
        {
            int countofTest = 1;
            int duration = stress_time/(countofTest);
            json pow;
            pow["gpu"] = json::array();
            saveback(benchmarks, pow, "terrain", duration, "bloom=false" );
            root["test"]["pow"] = pow;
        }
        {
            int countofTest = 1;
            int duration = stress_time/(countofTest);
            json cpu;
            cpu["gpu"] = json::array();
            saveback(benchmarks, cpu, "terrain", duration, "show-stats=false" );
            root["test"]["cpu"] = cpu;
        }
        {
            int countofTest = 1;
            int duration = stress_time/(countofTest);
            json shader;
            shader["gpu"] = json::array();
            saveback(benchmarks, shader, "shading", duration, "show-stats=false");
            root["test"]["shader"] = shader;
        }
        {
            int countofTest = 1;
            int duration = stress_time/(countofTest);

            json shader;
            shader["gpu"] = json::array();
            saveback(benchmarks, shader, "blank", duration, "show-stats=false");
            root["test"]["blank"] = shader;
        }
        {
            int countofTest = 1;
            int duration = stress_time/(countofTest);

            json usage;
            usage["gpu"] = json::array();
            saveback(benchmarks, usage, "refract", duration, "show-stats=false");
            root["test"]["usage"] = usage;
        }
    }
    root["selected"] = modeName;
    saveFile(file,root);
}

/**
 * Function:        int editJsonConfig(const char *key, const char *value)
 * Description:     Modifies an existing JSON configuration file by updating or adding a key-value
 *                  pair under the "test" section. If the provided value is a valid JSON object,
 *                  it is parsed and stored as JSON; otherwise, it is stored as a string.
 * Parameters:      key: const char* - The key to be modified or added within the "test" section.
 *                  value: const char* - The new value to be assigned to the specified key.
 *                                       If it's a valid JSON structure, it is parsed accordingly.
 * Returns:         int - Returns 0 on successful modification; returns -1 if the JSON
 *                        configuration file is missing or cannot be loaded.
 */

int editJsonConfig(const char *key , const char *value ) {

    json root;
    #if defined(__x86_64__)
      std::string file = "/tmp/config.json";
    #else
      std::string file = "/storage/emulated/0/Android/data/org.gpu.glload.debug/files/lists/config.json";
    #endif

    std::vector<std::string> benchmarks;
     
    if(loadFile(file, root))
    {

      try {
          json prsdata = json::parse(value);
          root["test"][key] = prsdata;
      }
      catch(...)
      {
          root["test"][key] = value;
      }
      saveFile(file,root);
    }
    else
    {
        fprintf(stderr, "Json config file is missing\n");
        return -1;
    }
    return 0;
}

#endif
