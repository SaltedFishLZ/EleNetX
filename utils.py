"""Common utility functions
"""
import os
import sys

import matplotlib as mpl
import matplotlib.pyplot as plt 

if sys.version <= "3.7":
    try:
        from collections.abc import OrderedDict
    except ImportError:
        from collections import OrderedDict
else:
    OrderedDict = dict


def obj_attr_cat_to_int(objects:OrderedDict, category:str) -> OrderedDict:
    """Map objects' categorical attributes to int
    :param objects: all objects to be processed
    :param category: category keyword in attributes
    :param: a map from category keyword to int
    """
    categories = [objects[_id][category] for _id in objects]
    int_map = OrderedDict.fromkeys(categories)
    # fill in color id by enumeration
    for i, c in enumerate(int_map):
        int_map[c] = i
    return int_map

