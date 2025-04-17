from jinja2 import Environment, PackageLoader
from cmsis_svd import SVDParser
import argparse 
import os
import helper
import shutil

current_file_location = os.path.dirname(os.path.abspath(__file__))

size_map = {
    8: 'std::uint8_t',
    16: 'std::uint16_t',
    32: 'std::uint32_t',
    64: 'std::uint64_t'
}

access_map = {
    'read-only': 'AccessType::ReadOnly',
    'write-only': 'AccessType::WriteOnly',
    'read-write': 'AccessType::ReadWrite',
}

# Create argument parser for a input file and a output file
parser = argparse.ArgumentParser(description='Convert SVD file to a header file')
parser.add_argument('--svd-file', help='The input SVD file')
parser.add_argument('--header-file', help='The output header file')
args = parser.parse_args()

#svd_file = c
svd_file = 'svd/STM32F767.svd'
os.makedirs('generated', exist_ok=True)

parser = SVDParser.for_xml_file(svd_file)
device = parser.get_device(xml_validation=False)
device = device.to_dict()

assert device['address_unit_bits'] == 8, "Currently only 8-bit address unit is supported"

peripherals = [peripheral for peripheral in device['peripherals'] if peripheral['name'] == 'GPIOB']
peripherals = helper.replace_values(peripherals, "size", size_map)
peripherals = helper.replace_values(peripherals, "access", access_map)


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
otuput_file = f"generated/{device['name']}_pac.hpp"
with open(otuput_file, 'w') as f:
    f.write(output)

# get file location

shutil.copyfile(os.path.join(current_file_location, 'register.hpp'), 'generated/register.hpp')

'''

peripherals = helper.replace_values(device.peripherals, "size", size_map)

#peripherals = []
#for peripheral in device.peripherals:
#    peripherals.append(peripheral)


'''