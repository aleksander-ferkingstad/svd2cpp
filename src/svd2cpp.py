from jinja2 import Environment, PackageLoader
from cmsis_svd import SVDParser
import argparse 
import os

def get_cpp_type(size):
    type_map = {8: 'uint8_t', 16: 'uint16_t', 32: 'uint32_t', 64: 'uint64_t'}
    return f'std::{type_map.get(size, "uint32_t")}'

# Create argument parser for a input file and a output file
parser = argparse.ArgumentParser(description='Convert SVD file to a header file')
parser.add_argument('--svd-file', help='The input SVD file')
parser.add_argument('--header-file', help='The output header file')
args = parser.parse_args()

svd_file = 'svd/STM32L4x1.svd'
os.makedirs('generated', exist_ok=True)
otuput_file = 'generated/output.hpp'

parser = SVDParser.for_xml_file(svd_file)
device = parser.get_device(xml_validation=True)

peripherals = []

for peripheral in device.peripherals:
    peripherals.append(peripheral)

env = Environment(loader=PackageLoader('svd2cpp'),
                  trim_blocks=True,
                  lstrip_blocks=True,
                  autoescape=False)

template = env.get_template('svd_template.jinja')

output = template.render(
    peripherals=peripherals, 
    converter_version = '0.0.1', 
    input_file = svd_file.split('/')[-1],
)

with open(otuput_file, 'w') as f:
    f.write(output)