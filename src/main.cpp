#include "SDL.h"
#include "engine/core/core.h"
#include "testApp/testLayer.h"
#include "testApp/overlay.h"
int main(int argc, char* argv[]) {
   WindowSettings windowSettings;
   windowSettings.height=900;
   windowSettings.width=1600;
   windowSettings.targetFps = 144;
   Core core(windowSettings);
   core.RegisterLayer(std::unique_ptr<Layer> (new TestLayer()));
   core.RegisterLayer(std::unique_ptr<Layer> (new Overlay()));
   core.Run();
}