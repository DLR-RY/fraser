'''
Created on Sep 11, 2017

@author: Annika Ofenloch
'''

from lxml import etree as ET
import argparse
import yaml

class ModelVarsWriter():
    def __init__(self):
        self.model_vars = {}
        pass

    def write_vars(self, root):
        model_list = []
      
        
        for model in root.iter('Model'):
            model_dependencies = []
            if model.find('Dependencies/ModelReference') is not None:
                for dependency in model.findall('Dependencies/ModelReference'):
                    for model_id in root.iter('Model'):
                        if(model_id.get('id') == dependency.get('modelID')):
                            model_dependencies.append(model_id.find('Name').text)
            
            model_list.append({'name': model.find('Name').text, 'depends': model_dependencies })

        self.model_vars = {'models': model_list }
        
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
    
    model_vars_writer.write_vars(root)
    # Write YAML file
    noalias_dumper = yaml.dumper.SafeDumper
    noalias_dumper.ignore_aliases = lambda self, data: True
    with open("{0}/roles/initialisation/vars/main.yml".format(args.destination), 'w') as outfile:
        yaml.dump(model_vars_writer.get_model_vars(), outfile, default_flow_style=False, Dumper=noalias_dumper)

    print("YAML-file for the model variables was created under 'ansible/roles/initialisation/vars/'.")
  

