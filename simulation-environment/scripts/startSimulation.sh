#!/bin/bash

../src/configuration_server/configServer &
../src/queues/event_queue_1/eventQueue &
../src/subcomponents/model_1/model1 &
../src/subcomponents/model_2/model2 &
../src/simulation_model/simulationModel