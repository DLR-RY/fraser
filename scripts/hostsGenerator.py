'''
Created on Aug 14, 2017

@author: Annika Ofenloch
'''

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
            hostname = host.find("Name").text
            hostaddress = host.find("Address").text
            
            models = []
            for model in root.iter('Model'):
                hostreference = model.find("HostReference").get("hostID")
                if(hostreference == host.get('id')):
                    models.append("{0}".format(model.find("Name").text))
    
            self.config["all"]["{0} ansible_host={1} models='{2}'".format(hostname, hostaddress, '["{0}"]'.format('", "'.join(models)))] = None

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


