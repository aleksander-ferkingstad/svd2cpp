from jinja2 import Environment, PackageLoader
from cmsis_svd import SVDParser
import argparse 
import os
import helper
import shutil
import re
import xml_helper
import json

parser = argparse.ArgumentParser(description='Convert SVD file to a header file')
parser.add_argument('--svd-file', help='The input SVD file')
parser.add_argument('--generate-json-svd', action='store_true', help='Generate JSON SVD file', default=False)

args = parser.parse_args()
svd_file = args.svd_file

os.makedirs('generated', exist_ok=True)
current_file_location = os.path.dirname(os.path.abspath(__file__))

def register_width_func(value : str) -> str:
    size_map = {
        '8': 'std::uint8_t',
        '16': 'std::uint16_t',
        '32': 'std::uint32_t',
        '64': 'std::uint64_t'
    }
    assert value in size_map, f"Register width {value} not supported"
    return size_map.get(value,value)

def size_func(value : str) -> str:
    size_map = {
        '0x8': 'std::uint8_t',
        '0x10': 'std::uint16_t',
        '0x20': 'std::uint32_t',
        '0x40': 'std::uint64_t',
    }
    
    return size_map.get(value,value)

def access_func(value : str) -> str:
    access_map = {
        'read-only': 'AccessType::ReadOnly',
        'write-only': 'AccessType::WriteOnly',
        'read-write': 'AccessType::ReadWrite',
    }
    return access_map.get(value,value)

def description_func(value : str) -> str:
    text = value.replace('\n',' ')
    text = re.sub(r'\s+', ' ', text)
    return text.strip()

def strip_trailing_digit(value : str) -> str:
    return re.sub(r'\d+$', '', value)

def add_enums_to_register(register : dict) -> dict:
    enums = []
    enum = {}
    if( not isinstance(register['fields']['field'], list) ):
        print("Converting field to list")
        print(register['fields']['field'])
        register['fields']['field'] = [register['fields']['field']]
        
    for field in register['fields']['field']:    
        if field.get('enumeratedValues', False):
            if field['enumeratedValues'].get('enumeratedValues', False): # contains a enum definition
                enum = field['enumeratedValues'].copy()
                if( not isinstance(enum['enumeratedValue'],list) ):
                    enum['enumeratedValue'] = [enum['enumeratedValue']]
                if(len(enum) == 1) : 
                    enum = [enum]
                field['enumeratedValues'] =  {'@derivedFrom': enum['name']}
                enums.append(enum.copy())
    register['enums'] = enums
                
xml_helper.xml_to_json_file(svd_file)
device = xml_helper.xml_to_dict(svd_file)['device']

assert device['addressUnitBits'] == '8', "Currently only 8-bit address unit is supported"
peripherals = device['peripherals']['peripheral']

# only keep GPIOA
peripherals = [peripheral for peripheral in peripherals if 'GPIOB' in peripheral['name']]

peripherals = helper.replace_values(peripherals, "size", size_func)
peripherals = helper.replace_values(peripherals, "size", size_func)
peripherals = helper.replace_values(peripherals, "access", access_func)
peripherals = helper.replace_values(peripherals, "description", description_func)

for peripheral in peripherals:
    print(peripheral['name'])
    if(not peripheral.get('registers', False)):
        print(f"Peripheral {peripheral['name']} has no registers")
        continue
    for register in peripheral['registers']['register']:
        register = add_enums_to_register(register)

# Save peripherals to JSON file
json_file = svd_file.replace('.svd', '_modified.json')
with open(json_file, 'w') as file:
    json.dump(peripherals, file, indent=4)

env = Environment(loader=PackageLoader('svd2cpp'),
                  trim_blocks=True,
                  lstrip_blocks=True,
                  autoescape=False)

template = env.get_template('svd_template.jinja')

output = template.render(
    peripherals=peripherals, 
    converter_version = '0.0.1', 
    svd_filename = svd_file.split('/')[-1],
    register_width = register_width_func(device['width'])
)
otuput_file = f"generated/{device['name']}_pac.hpp"
with open(otuput_file, 'w') as f:
    f.write(output)

shutil.copyfile(os.path.join(current_file_location, 'register.hpp'), 'generated/register.hpp')
