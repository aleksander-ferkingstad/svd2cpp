from jinja2 import Environment, PackageLoader
import argparse 
import svd_parser

parser = argparse.ArgumentParser(description='Convert SVD file to a header file')
parser.add_argument('--svd-file', help='The input SVD file')
parser.add_argument('--generate-json-svd', action='store_true', help='Generate JSON SVD file', default=False)

args = parser.parse_args()
svd_file = args.svd_file
peripherals = svd_parser.parse_svd_file(svd_file)

'''
for peripheral in device.peripherals:

env = Environment(loader=PackageLoader('svd2cpp'),
                  trim_blocks=True,
                  lstrip_blocks=True,
                  autoescape=False)

template = env.get_template('svd_template.jinja')

peripherals = []

output = template.render(
    peripherals=peripherals, 
    converter_version = '0.0.1', 
    svd_filename = svd_file.split('/')[-1],
)

otuput_file = f"generated/{device['name']}_pac.hpp"

with open(otuput_file, 'w') as f:
    f.write(output)

shutil.copyfile(os.path.join(current_file_location, 'register.hpp'), 'generated/register.hpp')
'''