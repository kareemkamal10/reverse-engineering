# Crear estructura del proyecto en C++
mkdir mining-behavior-simulator
cd mining-behavior-simulator

# Crear archivos principales del proyecto
touch README.md
touch CMakeLists.txt
mkdir src
touch src/cpu_simulator.cpp src/cpu_simulator.h
touch src/memory_simulator.cpp src/memory_simulator.h
touch src/network_simulator.cpp src/network_simulator.h
touch src/evasion_techniques.cpp src/evasion_techniques.h
touch src/main.cpp

# Crear directorio para archivos JSON de configuración/planificación
mkdir planning
touch planning/project_plan.json
touch planning/simulation_config.json