# Pixelpart Runtime Library

The Pixelpart runtime library is a C++17 library for loading and simulating visual effects created with [Pixelpart](https://pixelpart.net/).

Pixelpart is a tool for designing real-time visual effects for games. This runtime library allows applications to load effect files exported from Pixelpart and simulate them at runtime. Rendering is **not** included and must be implemented by the host engine or application. However, functions for generating vertex data are provided by the library.

The runtime library serves as the foundation for several game engine plugins.

## Requirements

- C++17 compatible compiler
- CMake

## Building

The library is designed to be embedded directly into your project.
Using CMake, add the repository as a subdirectory and link against the library target:

```cmake
add_subdirectory(pixelpart-runtime)
target_link_libraries(main PRIVATE pixelpart-runtime)
```

| Option | Description | Default |
| - | - | - |
| `PIXELPART_RUNTIME_MULTITHREADING` | Enables multi-threaded simulation support | `OFF` |

## Usage

Below is a basic program that demonstrates how to load and simulate an effect.

```cpp
#include <pixelpart-runtime/Pixelpart.h>
#include <memory>
#include <iostream>
#include <fstream>

int main(int argc, char* argv[]) {
    pixelpart::EffectAsset effectAsset;

    try {
        std::ifstream file("effect.ppfx");
        effectAsset = pixelpart::deserializeEffectAsset(file);

        std::cout << "Successfully loaded effect" << std::endl;
    }
    catch(const std::exception& e) {
        std::cerr << "Failed to load effect: " << e.what() << std::endl;
        return 0;
    }

    pixelpart::SingleThreadedEffectEngine effectEngine(effectAsset.effect(),
        std::make_shared<pixelpart::DefaultParticleGenerator>(),
        std::make_shared<pixelpart::DefaultParticleModifier>(),
        1000);

    for(float t = 0.0f; t < 1.0f; t += 0.01f) {
        effectEngine.advance(0.01);
    }

    return 0;
}
```
