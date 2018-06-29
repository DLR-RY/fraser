#!/usr/bin/env pythonNone
# -*- coding: utf-8 -*-
#
# Copyright (c) 2017-2018, German Aerospace Center (DLR)
#
# This file is part of the development version of FRASER.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# Authors:
# - 2017-2018, Annika Ofenloch (DLR RY-AVS)

from lxml import etree as ET
import argparse
import yaml

class ModelVarsWriter():
    def __init__(self):
        self.model_vars = {}
        pass

    def write_vars(self, root, filename):
        model_list = []
        config_path = ""
        
        for model in root.iter('Model'):     
            model_list.append(model.find('Name').text)

        for config in root.iter('Configuration'):
            config_path = config.get('path')
        
        self.model_vars = {'models': model_list, 'hosts_config_filepath': filename, 'config_path': config_path}
        
    def get_model_vars(self):
        return self.model_vars

if __name__ == '__main__':
    args_parser = argparse.ArgumentParser(description='Process some integers.')
    args_parser.add_argument('-f', '--file' , dest='file', help="Add xml-file of models-configuration", required=True)
    args_parser.add_argument('-d', '--dest' , dest='destination', help="Add path for 'hosts'-file", required=True)
    args = args_parser.parse_args()

    model_vars_writer = ModelVarsWriter()
    filename = args.file

    tree = ET.ElementTree()
    tree.parse(filename)
    root = tree.getroot()
    
    model_vars_writer.write_vars(root, filename)
    # Write YAML file
    noalias_dumper = yaml.dumper.SafeDumper
    noalias_dumper.ignore_aliases = lambda self, data: True
    with open("{0}/inventory/group_vars/all/main.yml".format(args.destination), 'w') as outfile:
        yaml.dump(model_vars_writer.get_model_vars(), outfile, default_flow_style=False, Dumper=noalias_dumper)

    print("YAML-file for the model variables was created under 'ansible/roles/initialisation/vars/'.")
  

