from jinja2 import Environment, PackageLoader
import xml.etree.ElementTree as ET
import argparse 
import svd_parser
import shutil
import os

def group_peripherals_by_groupname(root):
    """
    Returns a dict: {groupname: [peripheral, ...]}
    If groupName is missing, uses peripheral name instead.
    """
    groups = {}
    for peripheral in root.findall('./peripherals/peripheral'):
        group_name = peripheral.findtext('groupName')
        if not group_name:
            group_name = peripheral.findtext('name')
        if group_name not in groups:
            groups[group_name] = []
        groups[group_name].append(peripheral)
    return groups

if not os.path.exists('generated'):
        os.makedirs('generated')

parser = argparse.ArgumentParser(description='Convert SVD file to a header file')
parser.add_argument('--svd-file', help='The input SVD file')
parser.add_argument('--generate-json-svd', action='store_true', help='Generate JSON SVD file', default=False)

args = parser.parse_args()
svd_file = args.svd_file

tree = ET.parse(svd_file)
root = tree.getroot()

svd_parser.flatten_svd_file(root, debug=False)
groups = group_peripherals_by_groupname(root)

for groupname, peripherals in groups.items():
    
    env = Environment(loader=PackageLoader('svd2cpp'),
                  trim_blocks=True,
                  lstrip_blocks=True,
                  autoescape=False)
    
    template = env.get_template('svd_template.jinja')

    output = template.render(
        peripherals=peripherals, 
        converter_version = '0.0.1', 
        svd_filename = svd_file.split('/')[-1],
    )

    otuput_file = f"generated/{groupname}.hpp"
    with open(otuput_file, 'w') as f:
        f.write(output)
    
    current_dir = os.path.dirname(os.path.abspath(__file__))
    shutil.copyfile(os.path.join(current_dir, 'register.hpp'), 'generated/register.hpp')
    