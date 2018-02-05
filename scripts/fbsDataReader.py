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