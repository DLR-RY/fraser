#!/usr/bin/env pythonNone
# -*- coding: utf-8 -*-
#
# Copyright (c) 2019, German Aerospace Center (DLR)
#
# This file is part of the development version of FRASER.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# Authors:
# - 2019, Annika Ofenloch (DLR RY-AVS)

from lxml import etree as ET
import argparse
import os

class BashScriptWriter():
    def __init__(self):
        pass

    def write_start_simulation_script(self, root, filename):
        with open("{0}/create_default_configurations.sh".format(args.destination), 'w+') as create_default_configurations_file:
            create_default_configurations_file.write("#!/bin/bash\n\n")

            for model in root.iter('Model'):
                binary_path = os.path.relpath(model.get('path') + "/build/bin/*", "../")
               	model_id = model.get('id')
               	
                if model_id == "configuration_server":
                    create_default_configurations_file.write(binary_path + " --config-file " + filename + " &\n")

                if model_id == "logger":
                    create_default_configurations_file.write(binary_path + " --log-files-path " + "logs/ &\n")

                if model_id not in "simulation_model, configuration_server, logger":
                    create_default_configurations_file.write(binary_path + " -n " + model_id + " & \n")

            for model in root.iter('Model'):
                binary_path = os.path.relpath(model.get('path') + "/build/bin/*", "../")
                model_id = model.get('id')
                
                if model_id == "simulation_model":
                    create_default_configurations_file.write(binary_path + " --create-config-files configurations/config_0/ \n")

        print("debug script for starting the simulation were created under 'debug-scripts/'.")

    def write_default_configurations_script(self, root, filename):
         with open("{0}/start_simulation.sh".format(args.destination), 'w+') as start_simulation_file:
            start_simulation_file.write("#!/bin/bash\n\n")
            
            config_path = ""
        
            for models in root.iter('Models'):     
           	    config_path = os.path.relpath(models.get('configPath'), "../")

            for model in root.iter('Model'):
                binary_path = os.path.relpath(model.get('path') + "/build/bin/*", "../")
               	model_id = model.get('id')

                if model_id == "configuration_server":
                    start_simulation_file.write(binary_path + " --config-file " + filename + " &\n")

                if model_id == "logger":
                    start_simulation_file.write(binary_path + " --log-files-path " + "logs/ &\n")

                if model_id not in "simulation_model, configuration_server, logger":
                    start_simulation_file.write(binary_path + " -n " + model_id + " & \n")

            for model in root.iter('Model'):
                binary_path = os.path.relpath(model.get('path') + "/build/bin/*", "../")
               	model_id = model.get('id')
                
                if model_id == "simulation_model":
                    start_simulation_file.write(binary_path + " --load-config " + config_path + "/\n")

         print("debug script for creating default configuration files were created under 'debug-scripts/'.")

if __name__ == '__main__':
    args_parser = argparse.ArgumentParser(description='Process some integers.')
    args_parser.add_argument('-f', '--file' , dest='file', help="Add xml-file of hosts-configuration", required=True)
    args_parser.add_argument('-d', '--dest' , dest='destination', help="Add path for 'debug bash scripts", required=True)
    args = args_parser.parse_args()

    bash_scripts_writer = BashScriptWriter()
    filename = args.file

    tree = ET.ElementTree()
    tree.parse(filename)
    root = tree.getroot()

    filename = os.path.relpath(filename, "../")
    bash_scripts_writer.write_start_simulation_script(root, filename)
    bash_scripts_writer.write_default_configurations_script(root, filename)
