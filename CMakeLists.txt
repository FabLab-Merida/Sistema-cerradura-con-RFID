cmake_minimum_required(VERSION 3.23)
project(proyecto_puertas)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)
include_directories(final)
include_directories(final/NodoPuerta)
include_directories(final/ServidorRecibidor)
include_directories(net_examples)
include_directories(proyectoEmisorRF24)
include_directories(receptor)

add_executable(proyecto_puertas
        final/NodoPuerta/NodoPuerta.ino
        final/ServidorRecibidor/ServidorRecibidor.ino
        net_examples/HandControllerVeroboard.ino
        net_examples/TrackControlProMicroTA8428K.ino
        proyectoEmisorRF24/proyectoEmisorRF24.ino
        receptor/receptor.ino)
