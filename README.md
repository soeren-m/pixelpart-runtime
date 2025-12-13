# Runtime Library for Pixelpart

[Pixelpart](https://pixelpart.net/) is a tool for designing visual effects for games. This library provides methods to load and simulate effects created with *Pixelpart*. Rendering, however, is not part of the library.

The library is written in C++17 and used as a basis for various game engine plugins.

## Building

Use *CMake* to build the library by including the source code in your program and linking the library like this:

```cmake
add_subdirectory(pixelpart-runtime)
target_link_libraries(main PRIVATE pixelpart-runtime)
```

## Usage

Below is a basic program that uses the Pixelpart library. It loads an effect from file and simulates 1 second of it printing how many particles are created at each point in time.

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
        std::shared_ptr<pixelpart::DefaultParticleGenerator>(new pixelpart::DefaultParticleGenerator()),
        std::shared_ptr<pixelpart::DefaultParticleModifier>(new pixelpart::DefaultParticleModifier()),
        1000);

    for(float t = 0.0f; t < 1.0f; t += 0.01f) {
        std::cout << "[" << t << "s] "
            << effectEngine.particleCount() << " particles" << std::endl;

        effectEngine.advance(0.01);
    }

    return 0;
}
```
