#!/usr/bin/env pythonNone
# -*- coding: utf-8 -*-
#
# Copyright (c) 2017, German Aerospace Center (DLR)
#
# This file is part of the development version of FRASER.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# Authors:
# - 2017, Annika Ofenloch (DLR RY-AVS)

from lxml import etree as ET
import configparser
import argparse

class InventoryWriter():
    def __init__(self):
        self.config = configparser.ConfigParser(allow_no_value=True)
        pass

    def write_hosts_to_ini(self, root):
        self.config["all"] = {}
        
        for host in root.iter('Host'):
            hostname = host.get("id")
            hostaddress = host.find("Address").text

            models = []
            for model in root.iter('Model'):
                hostreference = model.find("HostReference").get("hostID")
                if(hostreference == host.get('id')):
                    models.append("{0}".format(model.get("id")))
    
            self.config["all"]["{0} ansible_host={1} model_ids='{2}'".format(hostname, hostaddress, '["{0}"]'.format('", "'.join(models)))] = None

    def get_configuration(self):
        return self.config

if __name__ == '__main__':
    args_parser = argparse.ArgumentParser(description='Process some integers.')
    args_parser.add_argument('-f', '--file' , dest='file', help="Add xml-file of models-configuration", required=True)
    args_parser.add_argument('-d', '--dest' , dest='destination', help="Add path for 'hosts'-file", required=True)

    args = args_parser.parse_args()

    inv_writer = InventoryWriter()
    filename = args.file

    tree = ET.ElementTree()
    tree.parse(filename)
    root = tree.getroot()
    
    inv_writer.write_hosts_to_ini(root);
    
    with open("{0}/inventory/hosts".format(args.destination), 'w') as configfile:
        inv_writer.get_configuration().write(configfile)

    print("File 'hosts.ini' was created.")


