import xmltodict
import json

def xml_to_json(xml_file):
    with open(xml_file, 'r') as file:
        xml_data = file.read()
    dict_data = xmltodict.parse(xml_data)
    json_data = json.dumps(dict_data, indent=4)
    json_file = xml_file.replace('.svd', '.json')
    with open(json_file, 'w') as file:
        file.write(json_data)