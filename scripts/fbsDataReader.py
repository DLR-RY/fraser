'''
Created on Dec 20, 2017

@author: Annika Ofenloch
'''
import argparse
import functools

class DataReader():
    def __init__(self):
        pass
    
    def readFile(self, file):
        repls = ('table', ''), (' ', ''), ('{', '')
        with open (file, 'rt') as in_file:
            for line in in_file:
                if "table" in line:
                    data_name = functools.reduce(lambda a, kv: a.replace(*kv), repls, line)
                    print(data_name)
    
if __name__ == '__main__':
    args_parser = argparse.ArgumentParser(description='Process some integers.')
    args_parser.add_argument('-f', '--file' , dest='file', help="Add fbs-file of custom data", required=True)
    
    args = args_parser.parse_args()
    filename = args.file
    
    data_reader = DataReader()
    data_reader.readFile(filename)